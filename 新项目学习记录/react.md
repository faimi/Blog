1. 在没写`return(<div></div>)`的时候`component: lazy(() => import('../container/financialManagement'))`会报错
2. 起名要简单，最好不超过四个单词
3. JSX进行条件渲染
```jsx
render() {
  const isLoggedIn = this.state.isLoggedIn;
  return (
    <div>
      The user is <b>{isLoggedIn ? (<div></div>) : (<div></div>)}</b> logged in.
    </div>
  );
}
```
4. 传`modifyData`到ModifyModal组件中，如何在定义useState的时候就获取到`props.modifyData.menuId`数据？
5. 不要改入参