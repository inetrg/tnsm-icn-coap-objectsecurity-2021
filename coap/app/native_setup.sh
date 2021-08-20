declare -A NODES=(
    [tap0]="BE:63:A1:23:DF:98 fe80::bc63:a1ff:fe23:df98 0"
    [tap1]="92:32:29:2B:04:B8 fe80::9032:29ff:fe2b:4b8  1"
    [tap2]="82:B1:CF:D9:DC:A2 fe80::80b1:cfff:fed9:dca2 1"
    [tap3]="62:18:E6:A1:F2:24 fe80::6018:e6ff:fea1:f224 2"
    [tap4]="92:BE:38:F4:70:CC fe80::90be:38ff:fef4:70cc 2"
)

declare -a ROUTES=(
    "tap0 tap3 tap1 down"
    "tap0 tap4 tap1 down"

    "tap1 tap3 tap2 down"
    "tap1 tap4 tap2 down"
    "tap1 tap0 tap0 up"

    "tap2 tap3 tap3 down"
    "tap2 tap4 tap4 down"
    "tap2 tap0 tap1 up"

    "tap3 tap0 tap2 up"

    "tap4 tap0 tap2 up"
)
