// TH04 MAINE verdict labels and filenames, checked against decoded 0E53:07xx.
// Each array owns an address passed by the verdict renderer or loader.
extern "C" {
// Five 8-byte gaiji rank labels at 0E53:071B.
char grEASY[5][8] = {
    { 0x02, 0x02, 0x02, 0xAE, 0xAA, 0xBC, 0xC2, 0 },
    { 0x02, 0xB7, 0xB8, 0xBB, 0xB6, 0xAA, 0xB5, 0 },
    { 0x02, 0x02, 0x02, 0xB1, 0xAA, 0xBB, 0xAD, 0 },
    { 0xB5, 0xBE, 0xB7, 0xAA, 0xBD, 0xB2, 0xAC, 0 },
    { 0x02, 0x02, 0xAE, 0xC1, 0xBD, 0xBB, 0xAA, 0 },
};
// 点 | target 0E53:0744
char aU_[] = "\x93\x5F";
// ． | target 0E53:0747
char aBd[] = "\x81\x44";
// ％ | target 0E53:074A
char aBu[] = "\x81\x93";
// ． | target 0E53:074D
char aBd_0[] = "\x81\x44";
// ％ | target 0E53:0750
char aBu_0[] = "\x81\x93";
// 　　　　　　　 腕前判定 | target 0E53:0753
char aB_b_b_b_b_b_b[] = "\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x81\x40\x20\x98\x72\x91\x4F\x94\xBB\x92\xE8";
// 難易度 | target 0E53:076B
char aUqiUx[] = "\x93\xEF\x88\xD5\x93\x78";
// 最終得点 | target 0E53:0772
char aNPiuU_[] = "\x8D\xC5\x8F\x49\x93\xBE\x93\x5F";
// ミス回数 | target 0E53:077B
char aGGxi[] = "\x83\x7E\x83\x58\x89\xF1\x90\x94";
// ボム使用回数 | target 0E53:0784
char aGGaogcpi[] = "\x83\x7B\x83\x80\x8E\x67\x97\x70\x89\xF1\x90\x94";
// ゲーム達成率 | target 0E53:0791
char aGqbGatbrmcj[] = "\x83\x51\x81\x5B\x83\x80\x92\x42\x90\xAC\x97\xA6";
// 悪霊退治率 | target 0E53:079E
char aIlcSObcj[] = "\x88\xAB\x97\xEC\x91\xDE\x8E\xA1\x97\xA6";
// アイテム回収率 | target 0E53:07A9
char aGagcgegai[] = "\x83\x41\x83\x43\x83\x65\x83\x80\x89\xF1\x8E\xFB\x97\xA6";
// 得点アイテム最高点率 | target 0E53:07B8
char aUU_gagcgeganNv[] = "\x93\xBE\x93\x5F\x83\x41\x83\x43\x83\x65\x83\x80\x8D\xC5\x8D\x82\x93\x5F\x97\xA6";
// 気合い | target 0E53:07CD
char aLcnzvv[] = "\x8B\x43\x8D\x87\x82\xA2";
// 処理落ち率 | target 0E53:07D4
char aPicacovCj[] = "\x8F\x88\x97\x9D\x97\x8E\x82\xBF\x97\xA6";
// あなたの腕前 | target 0E53:07DF
char aVavVVSrso[] = "\x82\xA0\x82\xC8\x82\xBD\x82\xCC\x98\x72\x91\x4F";
// 回 | target 0E53:07EC
char aTimes[] = "\x89\xF1";
// 回 | target 0E53:07EF
char aTimes_0[] = "\x89\xF1";
// 点 | target 0E53:07F2
char aPoint[] = "\x93\x5F";
// _ude.txt | target 0E53:07F5
char a_ude_txt[] = "\x5F\x75\x64\x65\x2E\x74\x78\x74";
// ？？？？？？点 | target 0E53:07FE
char aBhbhbhbhbhbhu_[] = "\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x81\x48\x93\x5F";
// 処理落ちによる判定不可 | target 0E53:080D
char aPicacovVVcvsfT[] = "\x8F\x88\x97\x9D\x97\x8E\x82\xBF\x82\xC9\x82\xE6\x82\xE9\x94\xBB\x92\xE8\x95\x73\x89\xC2";
// ude.pi | target 0E53:0824
char aUde_pi[] = "\x75\x64\x65\x2E\x70\x69";
}
