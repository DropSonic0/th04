#!/usr/bin/env python3
"""Place a statically audited native OP.EXE in a disposable TH04 HDI."""

import sys

from prepare_th04_maine_diagnostic_hdi import main


if __name__ == "__main__":
    sys.argv[1:1] = ["--artifact", "op"]
    raise SystemExit(main())
