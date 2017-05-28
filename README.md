# mentohust_jmu

# 该版本只有我自己能用，只有我自己能用，只有我自己能用，我做不出通用的，不好意思。

# 如果 Github 显示效果不好的话  也可以到这个网址查看教程
http://codingstory.com.cn/mo-gai-mentohust-v4ban-ben-de-xin-de/

用 Mentohust 最初版的源代码加上 Fork 来的V4算法拼凑出的一个Mentohust.

适用于集美大学教育网接入学生

Ubuntu下实测认证成功

其他VPN服务更新中


### 前言
![](http://codingstory.com.cn/content/images/2016/09/1111-2.jpg)
>每一个问题都是一把锁，你要相信，世界上一定存在一把钥匙可以打开这把锁，你也一定能找到这把钥匙。

* 1、我改的这个版本，目前只适用于集美大学Linux下锐捷认证，而且目前只能是我自己认证成功，我分享的，是魔改的方法。  [源代码已上传Github](https://github.com/ShanQincheng/mentohust_jmu.git)

* 2、如果你也是集大的同学，我相信你只要自己抓包，更改mystate.c文件中的三个pk数组。预编译，编译，汇编，链接。你也能在Linux下认证成功
如果你不是集大的同学，我相信我的分享能让你在魔改Mentohust V4版本上，少走很多弯路  

* 3、**如果你成功了，请遵守 GPL 3.0 协议 ,谢谢**

***


### 开始

* 1、用到的工具有哪些  

* 2、锐捷的认证过程是怎样的

* 3、如何用 WireShark 抓锐捷认证包

* 4、分析锐捷认证包

* 5、对应的 C 代码具体位置在哪

* 6、V4 算法中 checkV4[ ] 数组如何提取

***

##### 用到的工具
* 1、[WireShark](https://www.wireshark.org/#download) 抓锐捷认证包的工具，**Mentohust Tool 抓不到包的全部内容，不可用。**

* 2、[Notepad++](https://notepad-plus-plus.org/download/v7.html) 很好用的文本编辑器，当然你可以用其他的

* 3、[WinHex](http://www.winhex.com/winhex/) 很好用的 16 进制编辑器，当然你可以用其他的

* 4、[Linux版本锐捷](http://user.jmu.edu.cn/SelfService/SoftDown.aspx) **此为集大版，请下载自己学校的锐捷 Linux 版本，因为每个学校虽然 V4算法一样，但用到的 Key 是不同的，这个 Key 需要自己到 Linux 版本的锐捷中去提取**



***

##### 锐捷的认证过程是怎么样的
锐捷是怎样认证的，它发送的认证数据包有哪些，这是必须要弄清楚弄明白的。否则看到那一堆 Mentohust 源代码只会是雾里看花。  
锐捷的认证是基于 EAP 协议的，也用到了 EAP 的扩展认证协议 EAPOL。EAPOL是基于802.1X网络访问认证技术发展而来，所以windows锐捷客户端里都有一个8021x.exe文件。  

**EAP协议相关资料**  
[EAP协议](http://baike.baidu.com/link?url=6VBBcv__1lRioZ2JRQj6QPgK50plVjDxgXtS1NkAzLMMQxDfRWQpB0OijrhpIIkfpdE8HighEx83k9Wl_Uyod8QCW2NxsGANdNxC5_18P6q)              
[EAPOL协议](http://baike.baidu.com /link?url=NNORQZnEBj_2WOCLU67g4c4STGaMgzbS9a0pExQQw2tuWzzqTB4t0UuVc9lL7FFFBHnq2R6HNxnjiZ5y8LGs9a)          

**简言之**   

* 1、客户端给服务端发一个Start包  
* 2、服务端回复一个Identity请求包，客户端发送一个Identity响应包  
* 3、服务端回复一个MD5-Challenge请求包。客户端发送一个MD5-Challenge响应包。
* 4、如果以上步骤都没有问题，服务端回复一个Success成功包，认证成功。如果任何时候服务端回复一个Failure包，这次认证都立刻失败。  

**认证成功的情况**
![](http://codingstory.com.cn/content/images/2016/09/---.png)
**认证失败的情况**
![](http://codingstory.com.cn/content/images/2016/09/----1.png)

我们要做的全部工作，就是将图中黑色标记的，发送给服务端的三个包，Start包，Identity包，
MD5-Challenge包，在本地制作好，然后依次发送出去。让服务端以为我们在用锐捷客户端认证，其实我们是在用 Mentohust 认证。**在制作这三个包时，包的长度，包中信息的位置及长度，都必须要和锐捷客户端认证时发送的一样，否则必然不能通过。**

***

##### 用 WireShark 抓锐捷客户端认证包

>Wireshark is the world's foremost network protocol analyzer. It lets you see what's happening on your network at a microscopic level. It is the de facto (and often de jure) standard across many industries and educational institutions.  
Wireshark development thrives thanks to the contributions of networking experts across the globe. It is the continuation of a project that started in 1998.

如何安装我就不介绍了

锐捷断开连接后，打开 WireShark ，上方工具栏

    Capture
    Options
    Manage Interfaces  （右下角）
只勾上你需要认证的网卡，最好用有线网卡，无线网卡我没试过
![](http://codingstory.com.cn/content/images/2016/09/Wire----.png)
然后 Start，双击网卡，然后在上方过滤器中输入 eapol ，右边点击箭头，此时 WireShark 正在抓包，并且过滤掉了除 EAP， EAPOL 协议以外的数据包了。
![](http://codingstory.com.cn/content/images/2016/09/-EAPOL---.png)

然后认证锐捷，我建议认证先用通用的服务接入方式，比如集大“教育网接入”是人人都可以用的服务接入方式。
一般此时 WireShark 就可以获取到锐捷认证的所有数据包了。**如果没有获取全的话，断开锐捷连接。点击 WireShark 左上角绿色鲨鱼鳍图标，重新开启抓包，然后再连接锐捷。第2次肯定能抓全了。**
![](http://codingstory.com.cn/content/images/2016/09/Start--1.png)
窗口上方可以选择各个包，下方便是被选包的 16 进制格式的数据。我选择Start包作为样例， Identity 包， MD5-Challenge 包一样的分析。**每次锐捷认证，三个包的长度都是固定不变的，自己改 Mentohust 代码的时候就以 WireShark 上抓到的包长度为准。**   

**我们需要关注的是 Start，Reponse Identity， Reponse MD5-Challenge 三个包，这三个包的内容是我们本地构造锐捷认证数据包所必需的内容。**   

将这三个包以C语言格式导出，用Notepad++查看就是以下效果。**这三个数组之后会在改Mentohust代码的时候会用到，很重要。**
![](http://codingstory.com.cn/content/images/2016/10/-C--.png)

**下一段 “分析锐捷认证包” 中说的 “注释掉” ，指的是注释掉这三段代码中的最开头的一部分。**




***

##### 分析锐捷认证包
以下是我分析了很多天的结果，每个学校数据包不同，当然包含的信息位置也不同。你得自己多抓包，多分析。有些是很容易看出来的，比如 MAC 地址，认证账号，服务商接入等，有些是很难看出来的，得到处查资料，拿前人的代码进行分析，这个需要耐心和一部分运气。**一定要分析清楚每一个信息的开始与结束位置，在改 Mentohust 代码的时候需要开始位置来确定偏移量的准确数据**

下面是我的分析结果，先贴出我们需要查找并替换的信息位置  
 
**Start包**
![](http://codingstory.com.cn/content/images/2016/10/Start---.png)

**Identity包**
![](http://codingstory.com.cn/content/images/2016/10/Identity---.png)

**MD5-Challage包**
![](http://codingstory.com.cn/content/images/2016/10/Challenge.png)

**其他的一些信息位置，拿 MD5-Challenge 包来分析（ Mentohust 代码中不需要管的）**
![](http://codingstory.com.cn/content/images/2016/10/----.png)

**以上的信息位置，都是我抓了很多次包，拿 Mentohust 的代码比对了很多次得出的结果。每个学校的锐捷认证包不一样，包的长度，信息的位置肯定也是不一样的。需要你自己去耐心比对。但是 V4 认证需要你找到的，就是前三幅图里的 6 个位置。**

* 1、 三个包中。每个包填充的开始位置，前面的注释掉，后面的保留。  （ 3 ）
* 2、 Identity 包和 MD5-Challenge包中，长 0x80 的 V4 效验值的开始位置。 （ 2 ）
* 3、 MD5-Challange 包中， 长 0x10 的 V4 密码效验值开始位置。 （ 1 ）   
   3 + 2 + 1 =6

**找到了，你的工作就完成一大半了。**


***

#####对应的 C 代码具体位置在哪
前面说了那么多，现在只是改改代码的事情了。最重要的工作已经做完了。打开 mystate.c 源文件。


**找到定义的三段 pk 数组。**
![](http://codingstory.com.cn/content/images/2016/10/--.png)
![](http://codingstory.com.cn/content/images/2016/10/pk2--.png)
![](http://codingstory.com.cn/content/images/2016/10/pk3--.png)

**找到 static int sendStartPacket() 函数**
![](http://codingstory.com.cn/content/images/2016/10/--Start-.png)

**找到 static int sendIdentityPacket() 函数**
![](http://codingstory.com.cn/content/images/2016/10/--Identity-.png)

**找到 static int sendChallengePacket() 函数**
![](http://codingstory.com.cn/content/images/2016/10/--MD5-Challenge-.png)


**将代码与抓包数据同时分析一下**
![](/content/images/2016/10/Start-----.png)
![](/content/images/2016/10/Identity-----1.png)
![](/content/images/2016/10/MD5-Challenge----.png)


***

##### V4 算法中 checkV4[ ] 数组如何提取
还没有说最核心的计算V4效验值的 checkV4.c 源代码，打开 checkV4.c 源代码

里面有两个数组
![](http://codingstory.com.cn/content/images/2016/10/checkV4--1.png)
![](http://codingstory.com.cn/content/images/2016/10/checkV4--2.png)
这两个数组里面的数据，是从 Linux 版本的锐捷一个文件里面用 WinHex 工具提取出来的，每个学校也不一样。可以看看我的两个数组，再看看别的学校 Mentohust 版本里这两个数组，找一找里面相同的一段，然后去 WinHex 里面查询出来。**我建议先看看我的数组和别的学校的数组的头尾是否相同，相同的话就查找对应的一段16进制数据，截取对应长度的16进制代码，复制为 C 数组的形式。**

拿我的第一个数组，array[1820]举例。该数组开头为, 0x36,0xE0,0x04,0x08,0x46,0xE0 .....
用 WinHex 打开 Linux 锐捷中， x32 文件下 rjsupplicant 文件，搜索这一段 16 进制
36E0040846E0
![](http://codingstory.com.cn/content/images/2016/10/WinHex--.png)
一共选择 1820 = 0x71C 个字节的数据，复制为 C 源文件，然后把这一段 16 进制代码替换掉原有的 array[1820]数组。array_1[2035]一样的操作。


***



### 感谢
**特别感谢华中科技大学 [liuqun](https://github.com/liuqun?tab=repositories) 开发了最初版本的 Mentohust**  
**特别感谢华中科技大学的 [hyrathb](https://github.com/hyrathb) 提供了核心的锐捷 V4 算法**  
**特别感谢中山大学的 [ivechan](https://github.com/ivechan) 对我的耐心指导**  
**特别感谢集美大学的 陈锦润 、杨磊 两位朋友对我的帮助**

***

### 结尾
至此，大功告成。编译链接运行即可。我的这种方法目前只能我自己的帐号在 Linux下 Mentohust认证登录。所以可以肯定的是我的方法是有一定的问题的，我没有做出一种通用的办法。也许会再努努力，也许搞一个自己的版本也满足了，看之后的心情吧。  

顺便贴上把 Mentohust 弄到路由器上运行的方法，链接如下。别人写的已经很好了，我就不再造轮子了。   
[mentohust v4版本编译及ipv6的配置教程](http://soundrain.net/2016/04/25/mentohust-v4%E7%89%88%E6%9C%AC%E7%BC%96%E8%AF%91%E5%8F%8Aipv6%E7%9A%84%E9%85%8D%E7%BD%AE/)   
[交叉编译mentohust实现锐捷认证共享上网 ](http://blog.csdn.net/warriorpaw/article/details/7990226)

希望你能从这篇博客里学到一些对你有用的东西，也欢迎在博客下面留言，提问。  

**谢谢。**

***

#以上
2016年10月1日











--------------------------------------------------------------------------------


完全没怎么看原来的代码，瞎改的。
加入了V4支持。具体算法看checkV4.c

所有hash都是从算法在rhash和ampheck借来魔改的……。

建议Arch Linux用户安装AUR中的mentohust-git
#使用方法

#安装mentohust：建议Ubuntu用户使用Deb包安装，Fedora用户使用RPM包安装

#如果确定自己可以使用xrgsu认证成功，打开终端输入sudo mentohust运行即可
如果不确定，切换到锐捷所在目录，然后输入以下命令：
sudo mkdir /etc/mentohust
sudo cp ./8021x.exe  /etc/mentohust
sudo cp ./W32N55.dll /etc/mentohust
然后打开终端输入sudo mentohust运行。如果认证失败，再切换到锐捷所在目录，输入以下命令：
sudo cp ./SuConfig.dat /etc/mentohust
然后打开终端输入sudo mentohust运行即可。
如果准确按以上步骤操作后还是认证失败，请下载MentoHUSTTool，在Windows下抓包并保存为data.mpf，
然后回到Linux，切换到data.mpf所在目录，输入以下命令：
sudo cp ./data.mpf /etc/mentohust
然后打开终端输入sudo mentohust -f/etc/mentohust/data.mpf -w运行即可。以后也只需输入sudo mentohust。

#您也可以按下面的方法操作：
1、静态IP用户请事先设置好IP；
2、打开终端，输入sudo mentohust，回车；
3、［正确］输入相应信息，如果认证成功，跳到第8步；如果提示“不允许使用的客户端类型”，按Ctrl+C结束认证；
4、打开终端，输入sudo mentohust -w -f'锐捷目录下任意文件路径'，回车；
5、如果认证成功，跳到第8步；如果提示“客户端完整性被破坏”，按Ctrl+C结束认证；
6、将锐捷安装目录下的SuConfig.dat重命名为其他名字；
7、打开终端，输入sudo mentohust，回车；
8、如果是动态IP且不是Linux，打开相应设置去更新IP。
9、以后认证只需打开终端，输入sudo mentohust，回车。
10、要修改某些参数请输入mentohust -h查看帮助信息并据此修改，例如修改密码sudo mentohust -pNewPassword -w，要临时修改则不加-w参数。

#如何退出:不以后台模式运行mentohust时，按Ctrl+C即可退出；后台运行时使用sudo mentohust -k退出认证。

#查看帮助信息请输入：mentohust -h
更详细的帮助信息请参考：http://wiki.ubuntu.org.cn/锐捷、赛尔认证MentoHUST

#修改参数请根据帮助信息操作，例如修改用户名和密码：sudo mentohust -uUsername -pPassword -w
指定某些参数仅对当次认证有效请根据帮助信息操作，例如临时修改用户名和密码：sudo mentohust -uUsername -pPassword

#如果提示缺少libpcap.so.0.x而在/usr/lib/目录下已存在一个libpcap.so.0.x.y，输入以下命令：
sudo ln -s libpcap.so.0.x.y /usr/lib/libpcap.so.0.x
否则请安装libpcap。

#权责声明
1、本程序所有涉及锐捷赛尔认证的功能均是来自前辈公开代码及抓包分析。
2、本程序于个人仅供学习，于他人仅供方便认证，不得使用本程序有意妨害锐捷赛尔认证机制及相关方利益。
3、一切使用后果由用户自己承担。
4、本程序不提供任何服务及保障，编写及维护纯属个人爱好，随时可能被终止。
5、使用本程序者，即表示同意该声明。谢谢合作。

源码可在项目主页获取：http://mentohust.googlecode.com/
联系作者：在http://mentohust.googlecode.com/留言或Email：mentohust@ehust.co.cc
