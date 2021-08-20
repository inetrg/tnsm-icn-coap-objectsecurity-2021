unset NODES
declare -A NODES=(
    [70]="02:BB:F7:65:10:6B:11:14 fe80::bb:f765:106b:1114 0"
    [2]="0A:5E:FD:65:10:6B:11:14 fe80::85e:fd65:106b:1114 0"
    [289]="52:AC:FD:65:10:6B:11:14 fe80::50ac:fd65:106b:1114 0"
    [288]="02:BE:FD:65:10:6B:11:14 fe80::be:fd65:106b:1114 1"
    [287]="2A:5E:FB:65:10:6B:11:14 fe80::285e:fb65:106b:1114 1"
    [286]="3A:B5:FB:65:10:6B:11:14 fe80::38b5:fb65:106b:1114 1"
    [285]="36:5F:F7:65:10:6B:11:14 fe80::345f:f765:106b:1114 1"
    [284]="0A:A9:FA:65:10:6B:11:14 fe80::8a9:fa65:106b:1114 1"
    [283]="26:5D:FA:65:10:6B:11:14 fe80::245d:fa65:106b:1114 2"
    [282]="32:5F:F3:65:10:6B:11:14 fe80::305f:f365:106b:1114 2"
    [281]="36:5E:FE:65:10:6B:11:14 fe80::345e:fe65:106b:1114 2"
    [280]="22:5D:F9:65:10:6B:11:14 fe80::205d:f965:106b:1114 2"
    [279]="06:5C:FD:65:10:6B:11:14 fe80::45c:fd65:106b:1114 2"
    [278]="3A:B4:F9:65:10:6B:11:14 fe80::38b4:f965:106b:1114 2"
    [277]="52:AA:F9:65:10:6B:11:14 fe80::50aa:f965:106b:1114 2"
    [276]="06:5E:F8:65:10:6B:11:14 fe80::45e:f865:106b:1114 2"
    [275]="06:AD:FA:65:10:6B:11:14 fe80::4ad:fa65:106b:1114 2"
    [274]="3A:5C:FA:65:10:6B:11:14 fe80::385c:fa65:106b:1114 2"
)

unset ROUTES
declare -a ROUTES=(
    "2 283 288 down"
    "2 282 288 down"
    "2 281 288 down"
    "2 280 288 down"
    "2 279 288 down"
    "2 278 288 down"
    "2 277 288 down"
    "2 276 288 down"
    "2 275 288 down"
    "2 274 288 down"

    "70 283 288 down"
    "70 282 288 down"
    "70 281 288 down"
    "70 280 288 down"
    "70 279 288 down"
    "70 278 288 down"
    "70 277 288 down"
    "70 276 288 down"
    "70 275 288 down"
    "70 274 288 down"

    "289 283 288 down"
    "289 282 288 down"
    "289 281 288 down"
    "289 280 288 down"
    "289 279 288 down"
    "289 278 288 down"
    "289 277 288 down"
    "289 276 288 down"
    "289 275 288 down"
    "289 274 288 down"

    "288 283 287 down"
    "288 282 287 down"
    "288 281 287 down"
    "288 280 287 down"
    "288 279 287 down"
    "288 278 287 down"
    "288 277 287 down"
    "288 276 287 down"
    "288 275 287 down"
    "288 274 287 down"
    "288 2 2 up"
    "288 70 70 up"
    "288 289 289 up"

    "287 283 286 down"
    "287 282 286 down"
    "287 281 286 down"
    "287 280 286 down"
    "287 279 286 down"
    "287 278 286 down"
    "287 277 286 down"
    "287 276 286 down"
    "287 275 286 down"
    "287 274 286 down"
    "287 288 288 up"
    "287 2 288 up"
    "287 70 288 up"
    "287 289 288 up"

    "286 283 285 down"
    "286 282 285 down"
    "286 281 285 down"
    "286 280 285 down"
    "286 279 285 down"
    "286 278 285 down"
    "286 277 285 down"
    "286 276 285 down"
    "286 275 285 down"
    "286 274 285 down"
    "286 2 287 up"
    "286 70 287 up"
    "286 287 287 up"
    "286 289 287 up"

    "285 283 284 down"
    "285 282 284 down"
    "285 281 284 down"
    "285 280 284 down"
    "285 279 284 down"
    "285 278 284 down"
    "285 277 284 down"
    "285 276 284 down"
    "285 275 284 down"
    "285 274 284 down"
    "285 2 286 up"
    "285 70 286 up"
    "285 286 286 up"
    "285 289 286 up"

    "284 283 283 down"
    "284 282 282 down"
    "284 281 281 down"
    "284 280 280 down"
    "284 279 279 down"
    "284 278 278 down"
    "284 277 277 down"
    "284 276 276 down"
    "284 275 275 down"
    "284 274 274 down"
    "284 2 285 up"
    "284 70 285 up"
    "284 285 285 up"
    "284 289 285 up"

    "283 284 284 up"
    "283 2 284 up"
    "283 70 284 up"
    "283 289 284 up"

    "282 284 284 up"
    "282 2 284 up"
    "282 70 284 up"
    "282 289 284 up"

    "281 284 284 up"
    "281 2 284 up"
    "281 70 284 up"
    "281 289 284 up"

    "280 284 284 up"
    "280 2 284 up"
    "280 70 284 up"
    "280 289 284 up"

    "279 284 284 up"
    "279 2 284 up"
    "279 70 284 up"
    "279 289 284 up"

    "278 284 284 up"
    "278 2 284 up"
    "278 70 284 up"
    "278 289 284 up"

    "277 284 284 up"
    "277 2 284 up"
    "277 70 284 up"
    "277 289 284 up"

    "276 284 284 up"
    "276 2 284 up"
    "276 70 284 up"
    "276 289 284 up"

    "275 284 284 up"
    "275 2 284 up"
    "275 70 284 up"
    "275 289 284 up"

    "274 284 284 up"
    "274 2 284 up"
    "274 70 284 up"
    "274 289 284 up"
)