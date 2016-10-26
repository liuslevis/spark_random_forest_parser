# Utils to Parse Spark Random Forest Debug String

## 数据准备

### 随机森林的描述文件

`forest_description.txt`，由 Spark 2.0.1 rfModel.toDebugString 生成，格式：

```
RandomForestClassificationModel (uid=rfc_56a93f217b1a) with 3 trees
  Tree 0 (weight 1.0):
    If (feature 57 <= 0.03)
     If (feature 40 <= 0.02)
      If (feature 43 <= 0.21)
       Predict: 3.0
       ...
```

### 待预测的 libsvm 文件

`forest_feed.libsvm`，libsvm 格式，每列由『标签 特征:值』组成，格式：

```
3 57:0.02 40:0.01 43:0.20
1 57:0.02 40:0.01 43:0.25
...
```

## 实现思路

对 `forest_description.txt` 进行语法解析，转化成树结构。在根据 `forest_feed.libsvm` 输出预测结果。

## 使用方法

```
./parse_random_forest forest_description.txt forest_feed.libsvm
```

## 输出

在 `main()` 函数中解析随机森林，读取 libsvm 文件，算得预测、权重数组：`vector<tuple<long, double> > pred_weights`，并打印如下：

```
will parse num of tree:3
did parse num of tree:3
parse error: num of lines should be even! tree:  Tree 2 (weight 1.0):

feature line:3 57:0.02 40:0.01 43:0.20
    prediction_weight: 3: 1
    prediction_weight: 0: 1

feature line:1 57:0.02 40:0.01 43:0.25
    prediction_weight: 1: 1
    prediction_weight: 0: 1
```