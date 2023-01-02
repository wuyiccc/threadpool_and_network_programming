# 1. 编译-debug模式
```
gcc 1.client.c -I common ./common/common.c  ./common/wechat.c ./common/wechat_ui.c  -lpthread -lncursesw -D _D -D _CLI -D UI
```

# 2. 编译-非debug模式

```
gcc 1.client.c -I common ./common/common.c  ./common/wechat.c ./common/wechat_ui.c  -lpthread -lncursesw -D _CLI -D UI
```