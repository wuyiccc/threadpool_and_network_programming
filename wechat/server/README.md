1. 安装ncurses环境
```
sudo apt-get install libncurses5-dev
```
# 2. 编译代码-debug模式
```
gcc 1.server.c -I ./common ./common/common.c ./common/wechat.c -lpthread -lncurses  -D _D
```

# 3. 编译代码-非debug模式
```
gcc 1.server.c -I ./common ./common/common.c ./common/wechat.c  -lpthread
```