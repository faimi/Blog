
## vue源码地址
[vue源码地址](https://github.com/vuejs/vue)

## Vue源码目录结构

```javascript
src                 # 源码全在这下面
├── compiler        # 编译相关的代码全在compiler目录
├── core            # 核心代码（灵魂部分）
   ├──components         # 内置组件
   ├──global-api         # 全局API
   ├──instance           # 渲染的辅助函数
   ├──observer           # 与响应式数据相关
   ├──util               # 工具方法
   ├──vdom               # vue2.0核心
├── platforms       # 不同平台的支持，可以跑在web，也可以配合weex跑native
   ├──web                # 浏览器的程序，放于平台相关的代码
   ├──wexx               # 跨端应用
├── server          # Node.js服务端渲染
├── sfc             # .vue 文件编译出JavaScript对象
├── shared          # 可共享的代码
```