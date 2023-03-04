1. 获取表单数据分为两种办法：
（1）第一种方法有两个条件：1、在`<Form.Item></Form.Item>`上加上`name`属性，2、在`<Button></Button>`上加上`htmlType="submit"`
（2）第二种方法的核心是`useForm`
```javascript
const [form] = Form.useForm();

<Form form={form}></From>

const formValues = form.getFieldsValue(); // 获取所有表单数据
const value = form.getFieldsValue('name'); // 获取单个表单数据
```

2. `<Form></Form>`的`<Input/>`组件需要初始化数据时，需要在`<Form></Form>`上加`initialValues={{Form.Item的name:数据}}`。
3. 当`<Form></Form>`要向`<Modal></Modal>`传递数据时，最好的办法就是使用`useState`。
4. 一个`<Form.Item></Form.Item>`只有一个组件就好。
5. 若`<Modal></Modal>`作为修改模块时，`<Form></Form>`组件不要用`form={form}`，因为这样会导致数据的延迟。
这是为什么呢？？？
6. 清空 Form 的 Select 的已选选项：`form.setFieldValue('FormItem的name', undefined)`；清空整个 Form ：`form.resetFields()`