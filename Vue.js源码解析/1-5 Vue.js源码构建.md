关键词：看住`build.js`文件 定义依赖 拿到配置 进行筛选 进行构建

涉及到的文件：

# Vue.js 源码构建

## Vue 用 Rollup 做构建

[Rollup](https://github.com/rollup/rollup) 和 webpack 一样都是构建工具，webpack 更强大，可以把图片、 js 等都编译成 JavaScript ，rollup 更适合 JavaScript 库的编译，只处理 js 部分，所以更轻量且代码更友好，所以 Vue.js 选了 rollup 做构建，它的构建相关配置都在 scripts 目录下。

## 构建脚本

vue 发布在 npm 上，每个 npm 包（相当于一个项目）都需要一个`package.json`文件来对他做描述，它的内容实际上是一个标准的 JSON 对象。我们通常会配置`script`字段作为 NPM 的执行脚本。

```json
{
  "name": "vue",
  "version": "2.6.13",
  "main": "dist/vue.runtime.common.js",
  "module": "dist/vue.runtime.esm.js",
  "scripts": {
    // 构建相关的任务就是这三个， "build" 是建立 web平台的 Vue.js，"build ssr" 是输出跟 server renderer 相关，"build weex" 是跟 weex 相关。作用都是构建 Vue.js，后面 2 条是在第 1 条命令的基础上，添加一些环境参数。
    "build": "node scripts/build.js",
    "build:ssr": "npm run build -- web-runtime-cjs,web-server-renderer",
    "build:weex": "npm run build -- weex",
  }
}
```

| 字段    | 含义                                                                                                                                            |
| ------- | ----------------------------------------------------------------------------------------------------------------------------------------------- |
| name    | 名字，是唯一的                                                                                                                                            |
| version | 版本                                                                                                                                            |
| main    | npm 包入口，`import "vue"`时会通过 main 查找入口                                                                                                                                            |
| module  | module 和 main 非常类似，在 webpack2 以上是把 module 做默认入口，也可以说 Vue.js 的默认入口是 esm.js 文件                                                                                        |
| scripts | npm 提供了 npm scripts ， scripts定义了很多脚本，每个脚本都是一个任务，通过 npm run 值（dev，bulid）可以进行各种不同的脚本执行不同的任务。|

通过构建生成的目标代码在 dist 文件下，默认已经构建了很多版本的 Vue.js 。

## 构建过程

为什么可以构建如此多版本的 Vue.js ？那就要先了解它的构建过程。例如执行`npm run build`，实际上是执行了`node scripts/build.js`脚本，也就是跑了 scripts 文件夹下`build.js`文件的 js 。

### （1）定义依赖的模块

```javascript
// scripts/build.js
const fs = require('fs')
const path = require('path')
const zlib = require('zlib')
const rollup = require('rollup')
const terser = require('terser')

if (!fs.existsSync('dist')) {
  fs.mkdirSync('dist')
}
```

### （2）从`config.js`配置文件中拿到构建所需要的所有的配置

```javascript
// scripts/build.js
let builds = require('./config').getAllBuilds()
```

`config.js`文件底部暴露了一个方法`exports.getAllBuilds()`，这是一个函数，通过`Object.keys(builds)`拿到一个数组，然后通过`map()`方法调用`genConfig()`函数

```javascript
// scripts/config.js
exports.getAllBuilds = () => Object.keys(builds).map(genConfig)
```

> 1、Object.keys() 方法会返回一个由一个给定对象的自身可枚举属性组成的**数组**
>
> 2、枚举是指一一列举列
>
> 3、map的三个参数：数组元素，元素索引，原数组本身`arr.map((currentValue,index,array)=>{})`

#### `builds`

其实`builds`是一个对象，每个`key`对应一个对象，其实就是不同版本 Vue.js 编译的配置

`entry`：入口，通过`resolve()`函数传字符串`web/entry-runtime.js`
`dest`：目标
`format`：文件格式，通过不同的`format`可以构建不同的版本
`banner`：局部变量，定义注释

##### `entry`和`dest`

所以最后以`entry-runtime.js`为编译入口，最后生成`vue.runtime.common.dev.js`文件

```javascript
// scripts/config.js
const builds = {
  'web-runtime-cjs-dev': {
    entry: resolve('web/entry-runtime.js'),
    dest: resolve('dist/vue.runtime.common.dev.js'),
    format: 'cjs',
    env: 'development',
    banner
  }
}
```

`resolve()`函数接收一个参数，在`entry`中`base`取`/`前面第一个值`web`，`p.slice(base.length + 1)`返回`entry-runtime.js`

在`dest`中`base`取值`dist`，在`alias.js`里没有`dist`，于是直接返回当前目录（`__dirname`）的大目录(`'../'`)，找到`vue.runtime.common.dev.js`文件

```javascript
// scripts/config.js
const aliases = require('./alias')
const resolve = p => {
  const base = p.split('/')[0]
  if (aliases[base]) {
    return path.resolve(aliases[base], p.slice(base.length + 1))
  } else {
    return path.resolve(__dirname, '../', p)
  }
}
```

`aliases()`函数在`alias.js`文件最后导出一个对象，对象里有很多`key`，通过`resolve()`函数返回字符串，字符串是一个目录，`resolve()`调用了`path.resolve()`，`path.resolve()`是node.js提供的一个路径解析的方法，`__dirname`是指当前的目录，并往上去找一级，找到大目录，再传参数`p`

因此`alias.js`文件是提供了一个到最终真实文件地址的一个映射关系

```javascript
// scripts/alias.js
const resolve = p => path.resolve(__dirname, '../', p)

module.exports = {
  vue: resolve('src/platforms/web/entry-runtime-with-compiler'),
  compiler: resolve('src/compiler'),
  core: resolve('src/core'),
  shared: resolve('src/shared'),
  web: resolve('src/platforms/web'),
  weex: resolve('src/platforms/weex'),
  server: resolve('src/server'),
  sfc: resolve('src/sfc')
}
```

##### `format`

`format`属性表示构建的格式，`cjs`表示构建出来的文件遵循 CommonJS 规范，`es`表示构建出来的文件遵循 ES Module 规范。`umd`表示构建出来的文件遵循 UMD 规范。

```javascript
// dist/vue.runtime.esm.js
export default Vue;
```

```javascript
// dist/vue.runtime.common.js
module.exports = require('./vue.runtime.common.dev.js')
```

```javascript
// dist/vue.js
(function (global, factory) {
  typeof exports === 'object' && typeof module !== 'undefined' ? module.exports = factory() :
  typeof define === 'function' && define.amd ? define(factory) :
  (global = global || self, global.Vue = factory());
}(this, function () { 'use strict';

```


##### `banner`

可以知道版本、创建者、License情况

```javascript
// scripts/cofig.js
const banner =
  '/*!\n' +
 `* Vue.js v${version}\n`+
 `* (c) 2014-${new Date().getFullYear()} Evan You\n`+
  ' * Released under the MIT License.\n' +
  ' */'
```

```javascript
// dist/vue.min.js
/*!
 * Vue.js v2.6.14
 * (c) 2014-2021 Evan You
 * Released under the MIT License.
 */
```

#### `genConfig()`

回到`scripts/cofig.js`中，数组调用`genConfig()`函数：
`opts`：`genConfig`获取`key`，`opts`是`builds`中`key`对应的对象
`config`：构造一个新的`config`对象，这个对象的数据结构才是真正的 Rollup 对应的配置结构，`entry`只是我们自己定义的`entry`，但是在 Rollup 中是叫`input`。所以`config`是 Rollup 最终要的配置。

**总的来说就是我们将`builds`经过映射和转化生成 Rollup 最终需要的配置，他也是一个数组。最终生成的数组返回到`build.js`文件下。**

```javascript
// scripts/cofig.js
function genConfig (name) {
  const opts = builds[name]
  const config = {
    input: opts.entry,
    external: opts.external,
    plugins: [
      flow(),
      alias(Object.assign({}, aliases, opts.alias))
    ].concat(opts.plugins || []),
    output: {
      file: opts.dest,
      format: opts.format,
      banner: opts.banner,
      name: opts.moduleName || 'Vue'
    },
    onwarn: (msg, warn) => {
      if (!/Circular/.test(msg)) {
        warn(msg)
      }
    }
  }
}
```

### （3）对配置进行过滤，过滤掉我们不需要的，就剩下我们需要编译的这样就可以构建出不同用途的 Vue.js 了

`process.argv[2]`对应的是`package.json`文件下的`-- weex`、`-- web`，如果有这些参数就会通过`filter`把之前不需要打包的过滤掉，如果没有参数就把`weex`给过滤掉。

```javascript
// scripts/build.js
if (process.argv[2]) {
  const filters = process.argv[2].split(',')
  builds = builds.filter(b => {
    return filters.some(f => b.output.file.indexOf(f) > -1 || b._name.indexOf(f) > -1)
  })
} else {
  // filter out weex builds by default
  builds = builds.filter(b => {
    return b.output.file.indexOf('weex') === -1
  })
}
```

### （4）调用`build()`函数做真正的构建过程

`build()`函数定义了一个`next()`方法，在`next()`方法执行的时候调用`buildEntry()`以及计数器`built`。

```javascript
// scripts/build.js
build(builds)

function build (builds) {
  let built = 0
  const total = builds.length
  const next = () => {
    buildEntry(builds[built]).then(() => {
      built++
      if (built < total) {
        next()
      }
    }).catch(logError)
  }
  next()
}
```

`buildEntry()`拿到了`builds`的`config`，`config`就是 Rollup 最终编译需要的`config`，`config`编译完就拿到了`bundle`，`bundle`通过`generate()`产生`output`，`output`对应生成的目标。可能会对`code`进行修改，比如判断是否要压缩的js，`isProd`的定义是文件以`min.js`结尾，就再做一次`terser.minify`压缩，最终调用`write()`方法最终生成到 dist 目录下。

```javascript
// scripts/build.js
function buildEntry (config) {
  const output = config.output
  const { file, banner } = output
  const isProd = /(min|prod)\.js$/.test(file)
  return rollup.rollup(config)
    .then(bundle => bundle.generate(output))
    .then(({ output: [{ code }] }) => {
      if (isProd) {
        const minified = (banner ? banner + '\n' : '') + terser.minify(code, {
          toplevel: true,
          output: {
            ascii_only: true
          },
          compress: {
            pure_funcs: ['makeMap']
          }
        }).code
        return write(file, minified, true)
      } else {
        return write(file, code)
      }
    })
}
```

**个人理解：就是 执行代码和定义代码一般写在 js 文件的上面，方便看执行过程，所以 function 一般写在 js 文件的后面部分。**
