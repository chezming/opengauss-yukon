# 维度扩展九交模型
## 什么是九交模型
九交模型用来描述2D平面中两个几何图形的拓扑关系。在四交模型（内部、边界）的基础上进一步将几何图形的属性划分为内部、边界、外部。
![模块依赖图](./1.png)
+ 点要素，内部是点，边界是空集，外部是平面上除点以外的所有其他部分。
+ 线要素，内部、边界和外部不容易划分，内部是以端点为界限的线的那一部分，边界是线性要素的端点，外部是平面中除内部和边界外的所有其他部分。
+ 面要素，内部是以环为边界的里面的那一部分；边界是环本身；外部是边界外的一切。

## 什么是维度扩展九交模型
维度扩展九交模型[（Dimensionally Extended 9-Intersection Model，DE9IM）](https://en.wikipedia.org/wiki/DE-9IM)是对九交模型的应用延展，可以表达更加多样化的空间拓扑关系。因此，使用以上对对象内部、外部和边界的定义，我们可以用一组要素的内部/边界/外部九个可能性交集维度表示任何一对空间要素之间的关系，并利用数学矩阵来表达。如下图所示，关于它们的交集的DE9IM矩阵如下：
![模块依赖图](./2.png)
进一步地，对于上例中的多边形，内部的交集是二维区域，因此矩阵的对应部分用"2"填充。边界仅在零维点处相交，因此对应矩阵部分用"0"填充。

当两个几何图形的这三个部分（内部/边界/外部）之间没有交集时，将用"F"填充矩阵中对应的部分。请注意，以上两个要素的边界实际上根本不相交（线的端点与多边形的内部相交，而不是与多边形的边界相交，反之亦然），因此B/B单元用"F"填充。
![模块依赖图](./3.png)

我们通过[（ST_Relate）](http://postgis.net/docs/ST_Relate.html)函数让计算机自动填写DE9IM矩阵。DE9IM矩阵的强大之处不在于生成它们，而在于使用它们作为匹配参数来查找彼此之间具有特定关系的几何图形。

## 查找具有空间关系的集合图形
前面的示例可以使用简单的矩形和直线进行简化，其空间关系与上面的多边形和线串的空间关系相同：
![模块依赖图](./4.jpg)
使用SQL生成DE9IM信息：
```sh
SELECT ST_Relate(
         'LINESTRING(1 2, 3 2)',
         'POLYGON((2 1, 5 1, 5 3, 2 3))'
);
```
输出结果如下：
```sh
101
0F0
212
```
## 示例实践
假设我们需要在街区（Blocks）和道路（Roads）的两组要素数据中，寻找符合要求的道路，该处需符合一下两个条件：

1.该道路必须位于街区内部

2.且该路口必须临街区边界的位置。

接下来我们利用DE9IM在数据库中找到所有符合这一规则的路口：

首先，在数据库中加入Blocks和Roads，按序插入数据：
```sh
CREATE TABLE Blocks ( id serial primary key, geom geometry );
CREATE TABLE Roads ( id serial primary key, good boolean, geom geometry );

INSERT INTO Blocks ( geom )
  VALUES ( 'POLYGON ((100 200, 140 230, 180 310, 320 380, 470 340, 460 120, 200 70))');

INSERT INTO Roads ( geom, good )
  VALUES
        ('LINESTRING (170 290, 205 272)',true),
        ('LINESTRING (120 215, 176 197)',true),
        ('LINESTRING (280 270, 340 250)',false),
        ('LINESTRING (450 195, 510 310)',false),
        ('LINESTRING (460 180, 520 200)',false),
```
两组要素数据如下图所示：
![模块依赖图](./6.png)

根据上述需求，符合要求的道路具有以下特点：
+ 道路内部与街区内部有一个线性（一维）相交
+ 道路边界与街区内部有一个点（0维）相交
+ 道路边界与街区边界也有一个点（0维）相交
+ 道路内部与街区外部没有相交（F）

所以关于街区（Blocks）和道路（Roads）的DE9IM矩阵类似于下图所示：
![模块依赖图](./5.png)

```sh
id | good | geom
----+------+------------------------------------------------------------------------------------
  1 | t 010200000002000000000000000040654000000000002072400000000000A069400000000000007140
  2 | t    | 0102000000020000000000000000005E400000000000E06A4000000000000066400000000000A06840
(2 rows)
```
(* 注意，ST_Relate的三参数版本（重载函数）的使用，如果前两个几何图形参数的关系与第三个DE9IM模型参数匹配，则返回ture；如果不匹配，则返回false。)

另外，对于更松散的匹配搜索，第三个参数允许DE9IM数据模型字符串使用通配符：
+	"*"表示"此单元格中的任何值都可以接受"
+	"T"表示"任何非假值（0、1或2）都可以接受"

例如，我们在示例图形中添加一个与街区边界具有二维相交的道路：
```sh
INSERT INTO Roads ( geom, good )
  VALUES ('LINESTRING (140 230, 150 250, 210 230)',true);
```
![模块依赖图](./7.png)

将新增的加到在ST_Relate函数中符合要求，则需要修改ST_Relate函数中的第三个参数，因为道路内部和街区边界相交是1或F的情况皆可，因此，我们使用"*"通配符覆盖所有情况。

![模块依赖图](./8.png)

SQL语句如下所示：
```sh
SELECT Roads.*
FROM Roads JOIN Blocks ON ST_Intersects(Roads.geom, Blocks.geom)
WHERE ST_Relate(Roads.geom, Blocks.geom, '1*F00F212');
```
输出以下结果：
```sh
id | good |                                                        geom

  1 | t    | 010200000002000000000000000040654000000000002072400000000000A069400000000000007140
  2 | t    | 0102000000020000000000000000005E400000000000E06A4000000000000066400000000000A06840
  7 | t    | 01020000000300000000000000008061400000000000C06C400000000000C062400000000000406F400000000000406A400000000000C06C40
(3 rows)
```
![模块依赖图](./11.png)
## 应用场景：地籍图斑数据质量校验
利用DE9IM模型对地籍数据库图斑进行拓扑关系分析，可大大降低数据检查的工作量，基于拓扑规则的拓扑关系验证方法合理有效。
首先，利用SuperMap iDesktopX桌面工具将地籍数据集（DCK_ZD.udb)导入到Yukon数据库中，如图所示
![模块依赖图](./9.png)
```sh
SELECT a.SmID, b.SmID
FROM dck_zd_1 a, dck_zd_1 b
WHERE ST_Intersects(a.smgeometry, b.smgeometry)
  AND ST_Relate(a.smgeometry, b.smgeometry, '2********')
  AND a.SmID != b.SmID
LIMIT 10;
```
输出以下结果：
```sh
smid | smid
------+------
    6 |  208
    7 |    8
    8 |    7
    8 |    9
    9 |    8
   10 |   11
   11 |   10
   15 |   16
   16 |   15
   27 |   28
(10 rows)
```
如图所示，可以快速查询到符合条件的数据，快速对地籍数据进行校验检查，实现高效查询符合查找需求的叠加地块：
![模块依赖图](./10.png)