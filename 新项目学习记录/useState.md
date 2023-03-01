1. `useState`定义的变量在刷新后会初始化。

2. 当在`useState`中初始化的是对象时，最好把对象的属性名和初始属性值都写上，然后更新对象时，可以采用es的解构赋值。但是打印对象时，发现对象仍然是初始值。
```typescript
const [modifyData, choosedModifyData] = useState({
  key: -1,
  categoryName: "",
  categoryType: "",
  fatherCategory: "",
});
const modifyCategoryName = (category: any) => {
  choosedModifyData((preChoosedModifyData) => ({
    ...preChoosedModifyData,
    key: category.key,
    categoryName: category.categoryName,
    categoryType: category.categoryType,
    fatherCategory: category.fatherCategory,
  }));
  console.log(modifyData) //初始值
};
```

如果不需要更新，直接全部覆盖的话，就不需要解构赋值，直接赋值就好

```typescript
const modifyCategoryName = (category: any) => {
  choosedModifyData({
    key: category.key,
    categoryName: category.categoryName,
    categoryType: category.categoryType,
    fatherCategory: category.fatherCategory,
  });
  console.log(modifyData) //初始值
};
```

3. `useState`数据是异步更新的，所以`console`没更新是正常的，所以无法在更新后面直接接着使用更新后的state数据。

4. 数组循环push

注意：`useState`不能为空数组，一定要加一个`{}`，否则会变成`never[]`

最好是用`temp`过度一下，否则直接`set`会留空白`{}`

```typescript
const [firstOptions, setFirstOptions] = useState([{}])
const reqList = async (page: { pageNum?: number; pageSize?number; } = { pageNum: 1, pageSize: 1000 }) => {
    await request.shoesin.materialCategoryList(page).then((data) => {
        let temp = []
        if (data.rows) {
            for (const row of data.rows) {
                if (row.type === 1) {
                    temp.push({ value: row.id, label: row.name })
                    // setFirstOptions(old => [...old, { value: row.id, label: row.name}])
                }
            }
            setFirstOptions(temp)
        }
    });
};
const reqListRef = useRef(reqList);
useEffect(() => { reqListRef.current(); }, []);
```

5. `useState`更新是不能返回同一个对象，像`push`什么的都不好用，就直接用一种方法：es6的展开符就好了

即便是`return [...old]`也没用

```typescript
setSecondOptions(old => { old.push({ value: row.id, label: row.name }); return old })
```

还得是！！！

```typescript
setFirstOptions(old => [...old, { value: row.id, label: row.name }])
```

所以不是慢的原因

6. 场景：一个修改功能的Modal组件，想利用`const [menuTypeValue, setMenuTypeValue] = useState(props.modifyData.menuType);`存储不同的`menuType`，但是打开过一次Modal之后，后面的`menuTypeValue`不会变了，即传进来的`props.modifyData.menuType`没有及时更新。

疑惑：`useState`是不是初始化就一次，后面初始化的内容变了也不会初始化了？
答：是的，[https://juejin.cn/post/6997020924205596702](https://juejin.cn/post/6997020924205596702)

为什么不能
```typescript
const [menuTypeValue, setMenuTypeValue] = useState(props.modifyData.menuType);setMenuTypeValue(props.modifyData.menuType)
```
[问题在于，setCounter函数在组件渲染时被调用、更新状态，并导致重新渲染，而且是无限重新渲染。](https://juejin.cn/post/7176637831199064123)

为什么要用`useState`？
因为随着`Modal`的关闭，`useState`的数据依旧还在。

解决办法：用`useEffect`去监听传进来的参数`props.modifyData`

```typescript
useEffect(() => {
    setMenuTypeValue(props.modifyData.menuType);
    setIsFrameValue(props.modifyData.isFrame);
    setIsCacheValue(props.modifyData.isCache);
    setVisibleValue(props.modifyData.visible);
    setStatusValue(props.modifyData.status);
}, [props.modifyData]);
```