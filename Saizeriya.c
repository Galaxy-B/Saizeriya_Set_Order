#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define MAINDISH 0  //主菜
#define STAPLE 1    //主食
#define SNACK 2     //小吃
#define APPETIZER 3 //前菜
#define DESSERT 4   //甜品

typedef struct Dish
{
    int number;     //菜品编号
    char name[30];  //菜品名称
    int price;      //菜品价格
    int property;   //菜品属性
    int marked;     //指示该菜品是否已被选中
}Dish, *DishPtr;

typedef struct List
{
    int cost;               //总花费   
    int dish_num;           //菜品的总数
    DishPtr dishlist[30];   //存放选中菜品信息的数组
    int is_bar;             //指示是否需要畅饮
}List, *ListPtr;

//用于初始化萨莉亚菜单的函数
DishPtr Init_Menu(char *FileName, int *amount);

//用于初始化菜单推荐构成的函数
int* Init_Proportion(int persons);

//用于初始化最终菜单的函数
void Init_List(ListPtr result);

//用于读入用户需求的函数
//输入合法时返回1，不合法时返回0
int Get_Condition(int *persons, int *budget);

//判断是否需要畅饮的函数
void Bar_Judge(ListPtr result, int persons, int budget);

//生成随机菜单的函数
void Random_List(ListPtr result, DishPtr menu, int *proportion, int amount);

//在特定属性的菜品里进行随机的函数
//入口参数：菜品属性  推荐数量  最终菜单  总菜单  总菜单菜品数量
void Random_in_Property(int property, int num ,ListPtr result, DishPtr menu, int amount);

//判断最终菜单总价是否满足需求的函数
//符合预算要求返回2     (总价在预算浮动5元以内)
//需要调整菜品时返回1   (总价在预算浮动5-15元范围内)
//需要加菜或减菜时返回0 (总价在预算浮动15元以上)
int Valid_Judge(ListPtr result, int budget);

//更换已有菜品的函数
void List_Adjust(DishPtr menu, ListPtr result, int amount, int budget);

//增减已有菜品的函数
void List_AddOrDe(DishPtr menu, ListPtr result, int amount, int budget);

//打印最终结果列表的函数
void Print_List(ListPtr result, int persons);

//释放所有申请空间的函数
void Free_Memory(DishPtr menu, ListPtr result, int *proportion);

int main(void)
{
    srand((unsigned)time(NULL));    //取随机数种子
    
    int persons;    //人数
    int budget;     //总预算
    int amount;     //菜单中菜品总数
    DishPtr menu = Init_Menu("SaizeriyaMenu.txt", &amount);
    
    int status;     //指示读取用户需求是否成功
    do
    {
        status = Get_Condition(&persons, &budget);
    } while (!status);
    int *proportion = Init_Proportion(persons);         //读取当前人数的推荐菜单组成

    char choice;
    ListPtr result = (ListPtr)malloc(sizeof(List));     //最终给出的菜单
    do
    {
        Init_List(result);                              //初始化最终菜单
        Bar_Judge(result, persons, budget);             //判断是否下单畅饮
        Random_List(result, menu, proportion, amount);  //生成第一轮的随机菜单
        while (Valid_Judge(result, budget) != 2)        //判断该菜单是否满足要求
        {
            if (Valid_Judge(result, budget) == 1)       //菜单菜品需要调整
            {
                List_Adjust(menu, result, amount, budget);
            }
            else                                        //菜单菜品需要增减
            {
                List_AddOrDe(menu, result, amount, budget);
            }
        }
        Print_List(result, persons);
        printf("若对此菜单不满意可输入0重新生成新的菜单:");
        getchar();
        scanf("%c", &choice);
    } while (choice == '0');
    Free_Memory(menu, result, proportion);
    return 0;
}

DishPtr Init_Menu(char *FileName, int *amount)
{
    FILE *fp = fopen(FileName, "r");
    
    fscanf(fp, "%d ", amount);
    DishPtr menu = (DishPtr)calloc(*amount, sizeof(Dish));
    for (int i = 0; i < *amount; i++)
    {
        fscanf(fp, "%d ", &menu[i].number);
        fscanf(fp, "%s", menu[i].name);
        fscanf(fp, "%d ", &menu[i].price);
        fscanf(fp, "%d ", &menu[i].property);
    }
    for (int i = 0; i < *amount; i++)
    {
        menu[i].marked = 0;
    }
    return menu;
}

int* Init_Proportion(int persons)
{
    //推荐的菜单构成：主菜 主食 小吃 前菜 甜品
    int recommend[9][5] = {{0, 0, 0, 0, 0},
                           {0, 1, 1, 0, 1},
                           {1, 2, 2, 1, 2},
                           {1, 2, 2, 2, 3},
                           {2, 3, 2, 2, 3},
                           {2, 4, 3, 3, 4},
                           {3, 4, 4, 3, 4},
                           {3, 5, 4, 4, 4},
                           {3, 5, 5, 4, 5},};
    int* proportion = (int*)calloc(5, sizeof(int));
    for (int i = 0; i < 5; i++)
    {
        proportion[i] = recommend[persons][i];
    }
    return proportion;
}

int Get_Condition(int *persons, int *budget)
{
    char temp[9];

    printf("欢迎光临萨门!\n");
    printf("请输入总用餐人数:");
    scanf("%s", temp);
    if (atoi(temp) == 0)
    {
        printf("Invalid Input!\n");
        return 0;
    }//若无法转换为整数则输入不合法并打印错误提示
    *persons = atoi(temp);
    if (*persons < 1 || *persons > 8)
    {
        printf("萨门不渡阴间人!\n");
        return 0;
    }

    printf("请输入预计人均消费:");
    scanf("%s", temp);
    if (atoi(temp) == 0)
    {
        printf("Invalid Input!\n");
        return 0;
    }//若无法转换为整数则输入不合法并打印错误提示
    *budget = atoi(temp);
    if (*budget < 10)
    {
        printf("萨门不渡穷光蛋!\n");
        return 0;
    }
    *budget = (*budget) * (*persons);
    if (*budget > 750)
    {
        printf("萨门不渡饭桶!\n");
        return 0;
    }
    
    return 1;
}

void Init_List(ListPtr result)
{
    result->cost = 0;
    result->dish_num = 0;
    result->is_bar = 0;
    for (int i = 0; i < 30; i++)
    {
        result->dishlist[i] = NULL;
    }
}

void Bar_Judge(ListPtr result, int persons, int budget)
{
    if (budget / persons >= 30)
    {
        result->cost += persons * 8;
        result->is_bar = 1;
    }
}

void Random_List(ListPtr result, DishPtr menu, int *proportion, int amount)
{
    for (int i = 0; i < 5; i++)
    {
        Random_in_Property(i, proportion[i], result, menu, amount);
    }
}

void Random_in_Property(int property, int num ,ListPtr result, DishPtr menu, int amount)
{
    int temp;
    for (int i = 0; i < num; )
    {
        temp = rand() % amount;         //随机选中菜单中的一个菜品
        if (menu[temp].property == property && menu[temp].marked == 0)
        {
            result->dishlist[result->dish_num] = &menu[temp];
            (result->dish_num)++;               //最终菜品数量增加
            result->cost += menu[temp].price;   //总价格增加
            i++;                                //计数器加一
        }//菜品属性符合要求且未被选中过
    }
}

int Valid_Judge(ListPtr result, int budget)
{
    if (-5 < result->cost - budget && result->cost - budget < 5)
    {
        return 2;
    }
    else if ((-15 <= result->cost - budget && result->cost - budget <= -5) || (5 <= result->cost - budget && result->cost - budget <= 15))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void List_Adjust(DishPtr menu, ListPtr result, int amount, int budget)
{
    int target = rand() % (result->dish_num);
    if (result->cost > budget)          //高于预算时更换成一个更低价的菜品
    {
        for (int i = 0; i < amount; i++)
        {
            if (menu[i].property == result->dishlist[target]->property && menu[i].price < result->dishlist[target]->price)
            {
                result->cost -= result->dishlist[target]->price;
                result->dishlist[target] = &menu[i];
                result->cost += result->dishlist[target]->price;
                break;
            }
        }
    }
    else                                //低于预算时更换成一个更高价的菜品
    {
        for (int i = 0; i < amount; i++)
        {
            if (menu[i].property == result->dishlist[target]->property && menu[i].price > result->dishlist[target]->price)
            {
                result->cost -= result->dishlist[target]->price;
                result->dishlist[target] = &menu[i];
                result->cost += result->dishlist[target]->price;
                break;
            }
        }
    }
}

void List_AddOrDe(DishPtr menu, ListPtr result, int amount, int budget)
{
    static int kind = 1;            //保证每次执行该函数时增加的是不同属性的菜品
    int target;
    if (result->cost > budget)      //高于预算时删去一个菜品  
    {
        target = rand() % (result->dish_num);
        result->cost -= result->dishlist[target]->price;
        result->dishlist[target] = result->dishlist[--(result->dish_num)];
        result->dishlist[result->dish_num] = NULL;
    }
    else                            //低于预算时增加一个菜品
    {
        for(;;)
        {
            target = rand() % amount;
            if (menu[target].property == kind)
            {
                result->dishlist[result->dish_num] = &menu[target];
                result->cost += menu[target].price;
                (result->dish_num)++;
                kind = (kind++) % 5;
                break;
            }
        }
    }
    
}

void Print_List(ListPtr result, int persons)
{
    printf("您今日的萨莉亚菜单为:\n");
    if (result->is_bar)
    {
        printf("1001 畅饮 x%d %d\n", persons, persons * 8);
    }
    for (int i = 0; i < result->dish_num; i++)
    {
        printf("%d ", result->dishlist[i]->number);
        printf("%s ", result->dishlist[i]->name);
        printf("%d\n",result->dishlist[i]->price);
    }
    printf("菜品总价为%d元\n", result->cost);
    printf("菜品编号及价格仅供参考 请以餐厅实际菜单为准!\n");
}

void Free_Memory(DishPtr menu, ListPtr result, int *proportion)
{
    free(menu);
    free(result);
    free(proportion);
}