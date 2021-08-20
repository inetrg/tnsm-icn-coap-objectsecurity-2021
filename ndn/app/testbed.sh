unset NODES
declare -A NODES=(
    [205]="1A:BB:F6:65:10:6B:11:14 /temperature 0"
    [208]="02:AA:FB:65:10:6B:11:14 /temperature/02:AA 1"
    [193]="32:AA:FA:65:10:6B:11:14 /temperature/32:AA 1"
    [204]="2A:AB:FB:65:10:6B:11:14 /temperature/2A:AB 1"
    [198]="32:5F:F9:65:10:6B:11:14 /temperature/32:5F 1"
    [294]="16:B5:F8:65:10:6B:11:14 /temperature/16:B5 1"
    [300]="1A:AA:F9:65:10:6B:11:14 /temperature/1A:AA 1"
    [203]="06:58:F6:65:10:6B:11:14 /temperature/06:58 1"
    [307]="32:5E:FA:65:10:6B:11:14 /temperature/32:5E 1"
    [293]="06:A9:F7:65:10:6B:11:14 /temperature/06:A9 1"
    [290]="32:A4:FA:65:10:6B:11:14 /temperature/32:A4 1"
    [299]="02:BE:F2:65:10:6B:11:14 /temperature/02:BE 1"
    [195]="36:AC:FD:65:10:6B:11:14 /temperature/36:AC 2"
    [188]="02:AE:F6:65:10:6B:11:14 /temperature/02:AE 2"
    [189]="06:5C:FC:65:10:6B:11:14 /temperature/06:5C 2"
    [216]="22:B5:FA:65:10:6B:11:14 /temperature/22:B5 2"
    [219]="26:5D:F6:65:10:6B:11:14 /temperature/26:5D 2"
    [295]="06:BB:F4:65:10:6B:11:14 /temperature/06:BB 2"
    [297]="32:A5:F7:65:10:6B:11:14 /temperature/32:A5 2"
    [303]="32:AC:FA:65:10:6B:11:14 /temperature/32:AC 2"
    [304]="1A:AB:F9:65:10:6B:11:14 /temperature/1A:AB 2"
    [305]="1A:AD:F6:65:10:6B:11:14 /temperature/1A:AD 2"
    [312]="06:5E:FB:65:10:6B:11:14 /temperature/06:5E 2"
    [313]="02:5E:FB:65:10:6B:11:14 /temperature/02:5E 2"
)

unset ROUTES
declare -a ROUTES=(
    "205 195 203 down"
    "205 188 203 down"
    "205 189 203 down"
    "205 216 204 down"
    "205 219 204 down"
    "205 295 204 down"
    "205 297 204 down"
    "205 303 204 down"
    "205 304 204 down"
    "205 305 204 down"
    "205 312 204 down"
    "205 313 204 down"
    
    "203 195 198 down"
    "203 188 198 down"
    "203 189 198 down"

    "204 216 208 down"
    "204 219 208 down"
    "204 295 290 down"
    "204 297 290 down"
    "204 303 290 down"
    "204 304 290 down"
    "204 305 290 down"
    "204 312 290 down"
    "204 313 290 down"

    "198 195 195 down"
    "198 188 193 down"
    "198 189 193 down"

    "208 216 216 down"
    "208 219 219 down"

    "290 295 293 down"
    "290 297 293 down"
    "290 303 294 down"
    "290 304 294 down"
    "290 305 294 down"
    "290 312 294 down"
    "290 313 294 down"

    "193 188 188 down"
    "193 189 189 down"

    "293 295 295 down"
    "293 297 297 down"

    "294 303 299 down"
    "294 304 299 down"
    "294 305 300 down"
    "294 312 300 down"
    "294 313 300 down"

    "299 303 303 down"
    "299 304 304 down"

    "300 305 305 down"
    "300 312 307 down"
    "300 313 307 down"

    "307 312 312 down"
    "307 313 313 down"
)
