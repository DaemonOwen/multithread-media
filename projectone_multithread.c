///编译命令是 arm-linux-gcc projectone_multithread.c -o projectone_multithread -I /usr/local/arm/include/ -L /usr/local/arm/lib -lts -lpthread
#include <stdio.h>
#include <stdlib.h>//缺少头文件报错 error: expected expression before 'struct'
#include "tslib.h" //触摸屏库头文件
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>




#define DEV_PATH "/dev/event0"
#define START "/projectone/start.bmp"
#define BLANK "/projectone/blank.bmp"
#define ERROR "/projectone/error.bmp"
#define MAIN "/projectone/main.bmp"
#define MUSIC "/projectone/game/musicbrowse.bmp"
#define PICTURE "/projectone/game/picborwse.bmp"
#define SUCCESS "/projectone/success.bmp"
#define LOGINUI "/projectone/loginUI.bmp"
#define FB_PATH "/dev/fb0"
#define MUSICPATH "/projectone/game/"//指定某个目录，里面存放MP3，BMP格式的文件

//定义一个结构体，用于双向循环链表的节点，存放每一个搜索得到mp3路径的字符串或者bmp路径的字符串
struct  path
{
  char  strpath[50];
  struct path* prev;
  struct path *next;
};

void show_pic(const char *pathname,int x1,int y1);//刷图函数
int  loginUI(struct tsdev *ts,char*pass);//加载登录界面并且输入密码匹配，返回值代表登录能否成功，0代表成功，-1代表失败。
char get_num(struct tsdev *ts);
int mainUI(struct tsdev *ts);
int pass_check(char *pass);//匹配密码成不成功

unsigned int *mem_p; 
struct ts_sample samp;//全局的信息结构体变量
int lcd_fd;
unsigned int *mem_p; //定义无符号类型指针，用于存放映射地址
void LCD_Init(void);
void LCD_UnInit(void);
void mp3play(struct ts_sample samp,struct tsdev *ts,struct path*head);
void picborwse(struct ts_sample samp,struct tsdev *ts,struct path*head);
struct path * path_create(void);

void create(char *filename,struct path*head);
DIR* open_dir(char *pathname);
void dir_read(DIR *dir);
int is_scope(struct ts_sample samp,int x1,int y1,int x2,int y2);
  struct path*tail_insert(struct path *basic ,struct path *new);
  char *stringcat(char*str,struct path*p);
  void startinput(int i);
  void *autoplay(void *arg);


//声明全局变量，图片头节点
struct path*bmp_head;
//声明全局变量，mp3头节点
struct path*music_head;


int main(void)
{
  
  



  int m;
  //定义两个头节点
  bmp_head=path_create();
  music_head=path_create();

  //int m;//用于标记用户在主界面中选择的下一个界面
  char pass[6];
  //memset(pass,0,6);
  
  //配置好触摸屏的文件，随时准备好作为参数传值
    struct tsdev *ts;
  //信息结构体指针
  //打开目录


    DIR *dir=open_dir(MUSICPATH);
    //实例化双向循环链表路径
    dir_read(dir);

  LCD_Init();

  ts = ts_open(DEV_PATH,0);
  if(ts == NULL)
  {
    printf("ts_open failure!\n");
    return -1;
  }
  //触摸库插件配置 
  ts_config(ts);
  //

          
  //进入一个死循环

while(1)
{
    //只有通过密码验证才可以跳出死循环
     while(1)
             {
              memset(pass,0,6);
              m=loginUI(ts,pass);
               //首先加载登录loginUI，并且完成登录功能,密码输入不成功的话就输入到成功为止。
                if(m==1)//1表示密码验证成功。
                       {
                        

                        show_pic(SUCCESS,0,0);
                        sleep(2);

                         mainUI(ts);//进入主界面
                         break;
                       }
                  
                else 
                      {
                         show_pic(ERROR,0,0);//进入密码匹配不成功的错误提示页面并且逗留2秒钟
                          sleep(1);
                      }
             
                      
             }

         
              
  

}
       
             
            

  ts_close(ts);
  LCD_UnInit();
  return 0;
}

//LCD初始化
void LCD_Init(void)
{
  //打开屏幕
  lcd_fd = open(FB_PATH,O_RDWR);
  if(lcd_fd == -1)
  {
    printf("open lcd failure!\n");
    return ;
  }
  
  mem_p = mmap( NULL, 800*480*4 ,       //映射fb内存空间长度   
          PROT_READ | PROT_WRITE, //可读可写
          MAP_SHARED,         //进程间共享机制
          lcd_fd,         //lcd的文件描述符
          0);
  
  if(mem_p == MAP_FAILED)
  {
    printf("mmap failure!\n");
    return;
  }
}
//LCD撤消
void LCD_UnInit(void)
{
  close(lcd_fd);
  munmap(mem_p,800*480*4);
}


  
  //用于存放ts_open返回的值
  
  
  
  /*ts_read(ts, &samp, 1);
  printf("samp.x = %d,samp.y = %d,samp.pressure = %d\n",
      samp.x, samp.y, samp.pressure);
      */
  
  
  
      /*printf("samp.x = %d,samp.y = %d,samp.pressure = %d\n",
      samp.x, samp.y, samp.pressure);*/
    //获取键盘上的数字并且存放在password文件中
    //show_pic("start.bmp",i,int y1);

      
  
  //判断触摸屏所感知的x，y并成功返回对应的值。
      
int loginUI(struct tsdev *ts,char *pass)//1表示成功，-1表示失败
{
      
       //存放6位数字
    
       int i=-1;
   
        int x;
        int y;
    
 
       show_pic(LOGINUI,0,0);//加载loginUI背景
       
  memset(pass,0,6);
      //传ts变量给get_num（），把获取的数字赋给num，然后做判断，选择对应的功能，0-9赋值，10是登录，11是回删数字
      while(1)
              {
                
                ts_read(ts, &samp, 1);
                 if(samp.pressure) 
                   {
                    x=samp.x;
                    y=samp.y;
                    i++;

                      if(is_scope(samp,349,115,436,191))
                            {
                              strcat(pass,"1");
                            startinput(i);
                            }
          else if(x>444&&y>115&&x<534&&y<191)
           // else if(is_scope(samp,444,115,534,191))
                {
                              strcat(pass,"2");
                          startinput(i);
                            }
          else if(x>539&&y>115&&x<638&&y<191)
      //else if(is_scope(samp,538,115,638,191))
                {
                              strcat(pass,"3");
                              startinput(i);
                            }
         else if(x>350&&y>199&&x<437&&y<277)
           // else if(is_scope(samp,350,199,437,277))
                 {
                              strcat(pass,"4");
                 startinput(i);
                            }
          else if(x>444&&y>199&&x<532&&y<277)
        //else if(is_scope(samp,444,199,532,277))
                 {
                              strcat(pass,"5");
            startinput(i);
                            }
          else if(x>538&&y>199&&x<638&&y<277)
          //else if(is_scope(samp,538,199,638,277))
                 {
                              strcat(pass,"6");
                           startinput(i);
                            }
          else if(x>349&&y>285&&x<437&&y<360)
              //else if(is_scope(samp,349,285,437,360))
                 {
                              strcat(pass,"7");
                              startinput(i);
                            }
          else if(x>443&&y>285&&x<533&&y<360)
            //else if(is_scope(samp,443,285,533,360))
                 {
                              strcat(pass,"8");
                           startinput(i);
                            }
          else if(x>539&&y>285&&x<638&&y<360)
            //else if(is_scope(samp,539,285,638,360))
                {
                              strcat(pass,"9");
                              startinput(i);
                            }
          else if(x>645&&y>115&&x<772&&y<191)
           // else  if(is_scope(samp,645,115,772,191))
                     {
                       if(pass_check(pass)==1)
                                  return 1;
                      else
                                 return -1;
                     }
          else if(x>646&&y>199&&x<737&&y<277)
           // else if(is_scope(samp,646,199,737,277))
               {
                              strcat(pass,"0");
                              startinput(i);
                 }
          else if(x>645&&y>285&&x<773&&y<360)
                 //else if(is_scope(samp,645,285,773,360))
                     {
                                 i--;
                                 pass[i]='\0';
                                         //把数组下标往前一位，并且对应的密码位置刷图刷空白图片
                                             switch(i)
                                               {
                                                case 0:
                                                            show_pic(BLANK,360,20);
                                                                 break;
                                                case 1:
                                                           show_pic(BLANK,420,20);
                                                                 break;
                                                case 2:
                                                             show_pic(BLANK,483,20);
                                                                 break;
                                                case 3:
                                                             show_pic(BLANK,545,20);
                                                                 break;
                                                case 4:
                                                             show_pic(BLANK,606,20);
                                                                 break;
                                                case 5:
                                                            show_pic(BLANK,668,20);
                                                                 break;
                                                     default:
                                                     break;

                                                }
                                                i--;//因为i开头会再加
                                              
                     }   

                                                    
                   }
                        //i++;     
                           
                 }
  }      

void startinput(int i)
{
  switch(i)
                                               {
                                                case 0:
                                                            show_pic(START,360,20);
                                                                 break;
                                                case 1:
                                                            show_pic(START,420,20);
                                                                 break;
                                                case 2:
                                                             show_pic(START,483,20);
                                                                 break;
                                                case 3:
                                                             show_pic(START,545,20);
                                                                 break;
                                                case 4:
                                                             show_pic(START,606,20);
                                                                 break;
                                                case 5:
                                                            show_pic(START,668,20);
                                                                 break;
                                                    default:
                                                                 break;
                                               }
                                             
                                               return ;
}                         
void show_pic(const char *pathname,int x1,int y1)//x1代表显示图片的x轴起始位置，y1代表y轴
{

  int bmp_fd,i,x,y;
  unsigned short heigh,weight;
  unsigned char head_buff[54];//存储头信息的buff
  //打开图片
  bmp_fd = open(pathname,O_RDWR);
  if(bmp_fd == -1)
  {
    printf("open bmp failure!\n");
    return ;
  }
  
  read(bmp_fd,head_buff,54);
  //得到图片宽度
  weight = head_buff[19]<<8 | head_buff[18];
  
  //得到图片高度
  heigh = head_buff[23]<<8 | head_buff[22];
  //定义用于存放图片rgb的buff
  unsigned char bmp_buff[weight*heigh*3];
  unsigned int buff[weight*heigh];

  
  //将图片像素读到buffer当中
  read(bmp_fd,bmp_buff,sizeof(bmp_buff));
  
  
  for(i=0; i<weight*heigh; i++)
    buff[i] = bmp_buff[3*i+0] | bmp_buff[3*i+1]<<8 | bmp_buff[3*i+2]<<16;
  
  //以下代码段错误
  //图像点倒序
  for(y=0; y<heigh; y++)
    for(x=0; x<weight; x++)
      *(mem_p + (y+y1)*800 + (x+x1)) = buff[(heigh-1-y)*weight+x];
  
  close(bmp_fd);
  
}

int mainUI(struct tsdev *ts)//通过返回值1通知
{
     while(1)
       {
         show_pic(MAIN,0,0);//加载主界面图片
         ts_read(ts, &samp, 1);
         
               
    if(samp.x>502&&samp.y>28&&samp.x<727&&samp.y<178&&samp.pressure)
          {
                     
                    mp3play(samp,ts,music_head);
                      
                   
          }
     else if(samp.x>502&&samp.y>294&&samp.x<727&&samp.y<429&&samp.pressure)
         {
            picborwse(samp,ts,bmp_head);
                  
         }
     else if(samp.x>136&&samp.y>382&&samp.x<261&&samp.y<464&&samp.pressure)
               {
                  //结束函数
                   return 1;
               }
              
       }
    return 1;

}

int pass_check(char *pass)
{
  
  FILE *fp;
  char buffer[6];
  memset(buffer,0,6);
  fp=fopen("/projectone/pass.txt","r");
  if(fp==NULL)
     {
        perror("open file failed");
        return 0;
     }
            fread(buffer,6,1,fp);

      
           /* printf("%s\n",pass );
            */

            if(strcmp(pass,buffer)==0)
                    {
                      fclose(fp);
                      return 1;
                    }


            else
              {
               fclose(fp);
                 return -1;
              }
  

}
// 打开目录的函数，参数为一个目录路径
DIR* open_dir(char *pathname)
{
    DIR *dir;//定义一个目录指针
  struct dirent *tmp;//定义一个目录结构体指针，存放目录内容信息
             
              dir=opendir(pathname);
              if(dir==NULL)
              {
                printf("打开目录失败\n");
                return NULL;
              }
              
              return dir; 
}
//实例化图片的双向链表或者音乐的双向链表，传入文件路径名字，
//如果该函数的定义在dir_read()函数后面，会报错In function `dir_read':projectone.c:(.text+0x1088): undefined reference to `create'projectone.c:(.text+0x10cc): undefined reference to `create'

  void create(char *filename,struct path*head)
  {
    //先实例化每一个链表空节点
    struct path* new=path_create();
    //然后把绝对路径复制到结构体中的strpath
    strcpy(new->strpath,MUSICPATH);
    //然后把文件名拼接到结构体中的strpath
    strcat(new->strpath,filename);
    
    //然后把新结构体插入到双向链表中
    tail_insert(head,new);
    return ;
  }
//读取某路径下的文件，寻找.MP3文件，寻找.BMP文件，并且把找到的文件赋值给对应的链表
void dir_read(DIR *dir)
{
  int i;
  char *p;
       struct dirent*tmp;


              while((tmp=readdir(dir))!=NULL)
              {
                p=tmp->d_name;
               // printf("这是%s\n",tmp->d_name );
                //判断查找到的每一个文件的后缀是否是mp3或者bmp格式的文件
                //先判断有没有含有.mp3格式字符，或者bmp字符
                if(strstr(tmp->d_name,".mp3")||strstr(tmp->d_name,".bmp"))
                {
                  //再一步从字符的最后一个字符开始找到第一个.，然后往后判断是否等于mp3，或者bmp格式
                  for(i=strlen(tmp->d_name)-1;i>-1;i--)
                        {
                          if(p[i]=='.')

                              break;
                        }
                        //进一步确定后缀名是否是.mp3
                       if(strcmp(&p[i],".mp3")==0)
                              
                                create(tmp->d_name,music_head);
                                
                              
                               
                              
                                 
                          

                        //进一步确定后缀名是否是.bmp
                       if(strcmp(&p[i],".bmp")==0)
                          
                            create(tmp->d_name,bmp_head);
                        
                    
                         
                }

     
  }
  

 return ;
}

void mp3play(struct ts_sample samp,struct tsdev *ts,struct path *head)
{
  struct path*p=head;//把头结点指针赋值给p，没有数据
  p=p->next;//p现在是第一个有数据的结点
  char command[50];
   //加载背景图片
   show_pic(MUSIC,0,0);

    //ts_read(ts,&samp,1);
     //.定义一个字符数组用于存放拼接后的命令
  sprintf(command,"madplay %s &",p->strpath);
while(1)
           {

          ts_read(ts, &samp, 1);
  

         if(is_scope(samp,16,82,112,166))
        //开始,后台中madplay /路径xx.mp3的意思是播放路径中的xx.mp3
       {
         system(command);
       
       }
    else if(is_scope(samp,164,82,276,164))
        //暂停
        system("killall -STOP madplay &");
    else if(is_scope(samp,326,86,451,165))
        //继续
        system("killall -CONT madplay &");
    else if(is_scope(samp,496,87,614,164))
        //上一首
            {
           
             system("killall -KILL madplay");
             
              p=p->prev;
             if(p==music_head)
              p=p->prev;
          sprintf(command,"madplay %s &",p->strpath);
           system(command);
            }

    else if(is_scope(samp,663,86,776,165))
        //下一首
           {
            system("killall -KILL madplay");
            p=p->next;
            if(p==music_head)
             p=p->next;
           printf("564654656456\n");
           printf("%s\n",p->strpath );
          sprintf(command,"madplay %s &",p->strpath);

           system(command);

           }

    else if(is_scope(samp,328,224,457,309))
        //停止并且退出返回主页面
      {
        system("killall -KILL madplay");
        return;
      }  

           }
            
        return ;



}

void *autoplay(void *arg)
{
  struct path* p=(struct path*)arg;
  while(1)
       {
         sleep(1);
          if(p==bmp_head)
          p=p->next;
        show_pic(p->strpath,0,0);//实现刷图
        p=p->next;
      
       }
   
}
void picborwse(struct ts_sample samp,struct tsdev *ts,struct path*bmp_head)
{

 

  show_pic(PICTURE,0,0);
  struct path*p=bmp_head;
  p=p->next;//p指向第一个有值的数据
      
  while(1)
                 {
                  ts_read(ts,&samp,1);//必须抓取1次包才能执行结束。
          if(is_scope(samp,641,10,763,69))
      //使用多线程实现自动浏览功能，主线程一直在检测有没有其他压力有的话就结束子线程，从线程在实现自动浏览功能
                   {
                    int res;
                    pthread_t threadid;//创建一个线程ID
                   res=pthread_create(&threadid,NULL,autoplay,bmp_head);//最后一个参数传参bmp_head给线程函数

                   while(1)
                            {
                              sleep(1);
                                 ts_read(ts,&samp,1);
                      if(samp.pressure)
                      {
                         pthread_cancel(threadid);//取消子进程
                         break;
                      }
                            }
                    
                    }


  else if(is_scope(samp,640,189,760,250))
      //上一张
          {
            p=p->prev;
            if(p==bmp_head)
              p=p->prev;
            show_pic(p->strpath,0,0);
            printf("%s\n",p->strpath );
          }
  else if(is_scope(samp,640,281,760,338))
      //下一张
          {
            p=p->next;
            if(p==bmp_head)
              p=p->next;
            show_pic(p->strpath,0,0);
            printf("%s\n",p->strpath );
          }
  else if(is_scope(samp,640,381,758,447))
      //退出
      return;
                 }

             
}

int is_scope(struct ts_sample samp,int x1,int y1,int x2,int y2)//以后记得
{
   if(samp.x>x1&&samp.y>y1&&samp.x<x2&&samp.y<y2&&samp.pressure)

      return 1;

    else
           return 0;
}
//初始化
//创造节点
struct path*path_create(void)
{
  struct path *p;
       p=(struct path*)malloc(sizeof(struct path));
  if(p==NULL)
    return NULL;
  
  p->prev=p;
  p->next=p;
  return p;
}
//老师的basic是最后一个节点，但由于使用过程中，传最后一个比较麻烦，所以改为头节点
//把新的节点往后插，basic为头节点，basic前一个为最后一个节点
struct path*tail_insert(struct path *basic ,struct path *new)
{
  /*
     new->next=basic->next;
     new->prev=basic;
     basic->next->prev=new;
     basic->next=new;
              */

    new->next=basic;
    new->prev=basic->prev;
    basic->prev->next=new;
    basic->prev=new;
  printf("%s\n", music_head->prev->strpath);
}