# 1. 编译-debug模式
```
gcc 1.client.c -I common ./common/common.c  ./common/wechat.c ./common/wechat_ui.c  -lpthread  -D _D -D _CLI
```

# 2. 编译-非debug模式

```
gcc 1.client.c -I common ./common/common.c  ./common/wechat.c ./common/wechat_ui.c  -lpthread -D _CLI
```