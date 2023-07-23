![](./images/%E5%BE%AE%E4%BF%A1%E6%88%AA%E5%9B%BE_20230326141545.png)

背景：两个 `Form` ，一个用 AntD 的 `Form` ，一个用 Antd ProComponents 的 与 `FormItem` 配合的 `EditableProTable` 

要求1：用一个保存按钮响应两个 Form 的校验规则

问题1：点击添加一行数据，控制台无穷响应造成死循环

问题2：实现复制按钮

我认为的bug：
1、`EditableProTable`没有复制到下一行的功能
2、使用`formRef.current?.setFieldsValue`来进行复制，如果修改id，会变成不可编辑状态。