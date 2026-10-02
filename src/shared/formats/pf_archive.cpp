// TH04 PAR directory and streamed DOS file view. The resident interrupt entry
// is in pf_int21.asm; this unit owns only the archive and virtual handle state.
#pragma option -zCSHARED -3

#if defined(__TURBOC__) || defined(__MSDOS__)
# include <dos.h>
#endif
#if defined(_WIN32) || defined(__MSDOS__) || defined(__TURBOC__)
# include <io.h>
#else
# include <unistd.h>
# include <fcntl.h>
#include <stdint.h>
#include <stdio.h>

#ifndef PF_FN_LEN
# define PF_FN_LEN 13
#endif

#if !defined(__TURBOC__) && !defined(__MSDOS__)
#ifndef O_RDONLY
# define O_RDONLY 0
#endif
#ifndef O_WRONLY
# define O_WRONLY 1
#endif
#ifndef O_RDWR
# define O_RDWR   2
#endif

#endif
#endif

#include "src/shared/runtime/api.hpp"

#pragma pack(push, 1)
struct PfFrame {
	uint16_t es, ds, bp, di, si, dx, cx, bx, ax, ip, cs, flags;
};
#pragma pack(pop)

#if defined(__TURBOC__)
typedef char PfFrameSize[(sizeof(PfFrame) == 24) ? 1 : -1];
#endif

extern "C" int TH04_PASCAL pf_hook_install(void);
extern "C" void TH04_PASCAL pf_hook_remove(void);
extern "C" unsigned bbufsiz;
extern "C" unsigned pferrno;
extern "C" unsigned char pfkey;

static char archive_path[128];
static unsigned char __seg *directory;
static unsigned directory_count;
static unsigned archive_active;

struct VirtualFile {
	unsigned active;
	unsigned handle;
	unsigned char __seg *buffer;
	unsigned capacity, fill, next;
	unsigned packed_size, packed_read;
	unsigned declared_size, type;
	unsigned char aux;
	unsigned long packed_offset;
	unsigned long position;
	int previous;
	unsigned repeat;
};
static VirtualFile vf;

static unsigned get16(const unsigned char far *p)
{
	return (unsigned)p[0] | ((unsigned)p[1] << 8);
}

static unsigned long get32(const unsigned char far *p)
{
	return (unsigned long)get16(p) | ((unsigned long)get16(p + 2) << 16);
}

static unsigned char upper_ascii(unsigned char c)
{
	return ((c >= 'a') && (c <= 'z')) ? (unsigned char)(c - 32) : c;
}

static int name_equal(const char far *path, const unsigned char far *name)
{
	const char far *base = path;
	unsigned i;
	for (const char far *p = path; *p; p++) {
		if ((*p == '\\') || (*p == '/') || (*p == ':')) {
			base = p + 1;
		}
	}
	for (i = 0; i < 13; i++) {
		if (upper_ascii((unsigned char)base[i]) != upper_ascii(name[i])) {
			return 0;
		}
		if (name[i] == 0) {
			return 1;
		}
	}
	return base[13] == 0;
}

static void close_virtual(void)
{
	if (vf.active) {
		_dos_close(vf.handle);
		vf.active = 0;
	}
	if (vf.buffer) {
		hmem_free(vf.buffer);
		vf.buffer = 0;
	}
}

static int seek_payload(void)
{
	if (lseek(vf.handle, (long)vf.packed_offset, SEEK_SET) < 0) {
		return 0;
	}
	vf.fill = vf.next = vf.packed_read = vf.repeat = 0;
	vf.position = 0;
	vf.previous = -1;
	return 1;
}

static int physical_byte(void)
{
	unsigned got;
	if (vf.next == vf.fill) {
		unsigned request;
		if (vf.packed_read == vf.packed_size) {
			return -1;
		}
		request = vf.packed_size - vf.packed_read;
		if (request > vf.capacity) {
			request = vf.capacity;
		}
		if (_dos_read(vf.handle, (void far *)vf.buffer, request, &got) || !got) {
			return -1;
		}
		vf.packed_read += got;
		vf.fill = got;
		vf.next = 0;
	}
	return vf.buffer[vf.next++] ^ vf.aux;
}

static int decoded_byte(void)
{
	int value;
	if (vf.repeat) {
		vf.repeat--;
		return vf.previous;
	}
	value = physical_byte();
	if (value < 0) {
		return -1;
	}
	if ((vf.type == 0x9595) && (value == vf.previous)) {
		int count = physical_byte();
		if (count < 0) {
			return -1;
		}
		vf.repeat = (unsigned)count;
	}
	vf.previous = value;
	return value;
}

static unsigned read_virtual(unsigned char far *out, unsigned count)
{
	unsigned done = 0;
	while (done < count) {
		int value = decoded_byte();
		if (value < 0) {
			break;
		}
		out[done++] = (unsigned char)value;
		vf.position++;
	}
	return done;
}

static void success(PfFrame far *f, unsigned ax)
{
	f->ax = ax;
	f->flags &= ~1u;
}

static void failure(PfFrame far *f, unsigned dos_error)
{
	f->ax = dos_error;
	f->flags |= 1u;
}

extern "C" int TH04_PASCAL pf_dispatch(PfFrame far *f)
{
	unsigned ah = f->ax >> 8;
	if (!archive_active) {
		return 0;
	}
	if (ah == 0x3D) {
		const char far *path = (const char far *)MK_FP(f->ds, f->dx);
		unsigned i;
		int handle;
		if ((f->ax & 3u) != 0 || vf.active) {
			return 0;
		}
		for (i = 0; i < directory_count; i++) {
			const unsigned char far *entry = (const unsigned char far *)directory + i * 32u;
			if (!name_equal(path, entry + 3)) {
				continue;
			}
			printf("[TH04 PS3 PF] Opening archive member: %s\n", path ? path : "NULL");
			if (_dos_open(archive_path, 0, &handle)) {
				printf("[TH04 PS3 PF] Failed _dos_open backing archive %s for member %s\n", archive_path, path);
				failure(f, 2);
				return 1;
			}
			vf.handle = handle;
			vf.active = 1;
			vf.type = get16(entry);
			vf.aux = entry[2];
			vf.packed_size = get16(entry + 16);
			vf.declared_size = get16(entry + 18);
			vf.packed_offset = get32(entry + 20);
			vf.capacity = bbufsiz ? bbufsiz : 512;
			vf.buffer = (unsigned char __seg *)hmem_allocbyte(vf.capacity);
			if (!vf.buffer || !seek_payload()) {
				printf("[TH04 PS3 PF] Failed buffer alloc or seek_payload for member %s\n", path);
				close_virtual();
				failure(f, 8);
				return 1;
			}
			printf("[TH04 PS3 PF] Member %s ready (decl_size=%u, packed_size=%u, handle=%d)\n", path, vf.declared_size, vf.packed_size, handle);
			success(f, handle);
			return 1;
		}
		return 0;
	}
	if (!vf.active || (f->bx != vf.handle)) {
		return 0;
	}
	if (ah == 0x3F) {
		unsigned done = read_virtual((unsigned char far *)MK_FP(f->ds, f->dx), f->cx);
		success(f, done);
		return 1;
	}
	if (ah == 0x3E) {
		close_virtual();
		success(f, 0);
		return 1;
	}
	if (ah == 0x42) {
		long offset = (long)(((unsigned long)f->cx << 16) | f->dx);
		long base;
		long wanted;
		unsigned char discard[64];
		unsigned count;
		switch (f->ax & 0xffu) {
		case 0: base = 0; break;
		case 1: base = (long)vf.position; break;
		case 2: base = (long)vf.declared_size; break;
		default: failure(f, 1); return 1;
		}
		wanted = base + offset;
		if ((wanted < 0) || ((unsigned long)wanted > 65536UL)) {
			failure(f, 1);
			return 1;
		}
		if (((unsigned long)wanted < vf.position) && !seek_payload()) {
			failure(f, 1);
			return 1;
		}
		while (vf.position < (unsigned long)wanted) {
			count = (unsigned)((unsigned long)wanted - vf.position);
			if (count > sizeof(discard)) {
				count = sizeof(discard);
			}
			if (read_virtual(discard, count) != count) {
				failure(f, 1);
				return 1;
			}
		}
		success(f, (unsigned)vf.position);
		f->dx = (unsigned)(vf.position >> 16);
		return 1;
	}
	// Do not let DOS consume the backing archive handle through another API.
	if ((ah == 0x40) || (ah == 0x44) || (ah == 0x45) || (ah == 0x46)) {
		failure(f, 1);
		return 1;
	}
	return 0;
}

extern "C" void TH04_PASCAL pfend(void)
{
	if (archive_active) {
		pf_hook_remove();
		archive_active = 0;
	}
	close_virtual();
	if (directory) {
		hmem_free(directory);
		directory = 0;
	}
	directory_count = 0;
}

extern "C" void TH04_PASCAL pfstart(const unsigned char far *path)
{
	int handle;
	unsigned got, count, bytes, key, i;
	unsigned char header[16];
	unsigned char far *entries;
	printf("[TH04 PS3 PF] pfstart called: %s\n", path ? (const char*)path : "NULL");
	pfend();
	pferrno = 0;
	for (i = 0; i + 1 < sizeof(archive_path) && path[i]; i++) {
		archive_path[i] = path[i];
	}
	archive_path[i] = 0;
	if (path[i]) {
		pferrno = 1;
		printf("[TH04 PS3 PF] Path too long\n");
		return;
	}
	if (_dos_open(archive_path, 0, &handle)) {
		pferrno = 2;
		printf("[TH04 PS3 PF] ERROR: Failed _dos_open archive %s\n", archive_path);
		return;
	}
	if (_dos_read(handle, header, sizeof(header), &got) || got != sizeof(header)) {
		pferrno = 3;
		printf("[TH04 PS3 PF] ERROR: Failed reading header\n");
		_dos_close(handle);
		return;
	}
	bytes = get16(header);
	count = get16(header + 4);
	key = get16(header + 6);
	printf("[TH04 PS3 PF] Archive header parsed: count=%u, bytes=%u, key=%u\n", count, bytes, key);
	if ((count > 1023) || (bytes != (count + 1u) * 32u) || (key > 255)) {
		pferrno = 3;
		printf("[TH04 PS3 PF] ERROR: Invalid archive header params\n");
		_dos_close(handle);
		return;
	}
	pfkey = (unsigned char)key;
	directory = (unsigned char __seg *)hmem_allocbyte(bytes);
	if (!directory) {
		pferrno = 8;
		printf("[TH04 PS3 PF] ERROR: Directory hmem_allocbyte failed\n");
		_dos_close(handle);
		return;
	}
	if (_dos_read(handle, (void far *)directory, bytes, &got) || got != bytes) {
		pferrno = 3;
		printf("[TH04 PS3 PF] ERROR: Directory read failed\n");
		_dos_close(handle);
		pfend();
		return;
	}
	_dos_close(handle);
	entries = (unsigned char far *)directory;
	for (i = 0; i < bytes; i++) {
		unsigned char value = entries[i] ^ (unsigned char)key;
		entries[i] = value;
		key = (unsigned char)(key - value);
	}
	for (i = count * 32u; i < bytes; i++) {
		if (entries[i]) {
			pferrno = 3;
			printf("[TH04 PS3 PF] ERROR: Non-zero padding in directory\n");
			pfend();
			return;
		}
	}
	directory_count = count;
	archive_active = (unsigned)pf_hook_install();
	printf("[TH04 PS3 PF] pfstart SUCCESS: %u directory entries loaded from %s\n", directory_count, archive_path);
}
