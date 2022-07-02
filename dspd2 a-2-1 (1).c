#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Assumption: there are 3 trains that run daily with train id: 12139,12140,14439
//APPROACH : TREE OF TREE OF TREES (AVL TREES),date points to trains, trains points to Passenger
/*--------------------------------------------------------------------------------------------------------------------------------------*/
const int Train_capacity = 60;
const int NO_OF_SEATS = 15; //Number of seats in a travelling class
const int NO_OF_COACH = 3;  //Number of coaches in a travelling class
const int NO_OF_SEATS_PER_COACH = 5;
const int NO_OF_TRAVELLING_CLASSES = 4;     //AC1,2,3 and Sleeper
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//STRUCTURES
typedef struct date
{
    //Date of travel
    int date;
    int month;
    int year;
    struct date* left;
    struct date* right;
    int height;
    struct train* down;         //To point to tree of trains
}Date;

typedef struct train
{
    int train_id;
    int ALL_availability[4];        //Array of available seats in each travelling class(only 4 such classes) with indices 0:AC1 1:AC2 2:AC3 3:Sleeper
    int no_of_coach[4];              //Array to store number of coach for each travelling class
    int ** coach;                                           //Points to 2D Array where every row denotes a travelling class and column: seats empty in that bogie
    struct train* left;
    struct train* right;
    int height;
    struct Pass* down;      //To point to tree of passenger
}Train;

typedef struct Pass
{
    char name[20];
    int passenger_id;
    char train[20];
    char boarding_station[20];
    char travelling_class[10];
    char destination[20];
    int train_id;
    int bogie_no;
    int seat;
    int upgrade;
    //Date of applying
    int apply_date;
    int apply_month;
    int apply_year;
    struct Pass* left;
    struct Pass* right;
    int height;
}Passenger;
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//Date comparison
const int monthDays[12]
    = { 31, 28, 31, 30, 31, 30,
       31, 31, 30, 31, 30, 31 };
int countLeapYears(int y, int m)
{
    int years = y;
    if (m <= 2)
    {
         years--;
    }
    return years / 4 - years / 100 + years / 400;
}

int days_equivalents(int d, int m, int y)
{
    int n1 = y * 365 + d;
    for (int i = 0; i < m - 1; i++)
    {
        n1 += monthDays[i];
    }
    n1 += countLeapYears(y,m);
    return (n1);
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//To initialize a train node
void initialise_train(Train ** p)
{
    Train * t;
    t = *p;
    t->ALL_availability[0] = NO_OF_SEATS;
    t->ALL_availability[1] = NO_OF_SEATS;
    t->ALL_availability[2] = NO_OF_SEATS;
    t->ALL_availability[3] = NO_OF_SEATS;
    t->no_of_coach[0] = NO_OF_COACH;
    t->no_of_coach[1] = NO_OF_COACH;
    t->no_of_coach[2] = NO_OF_COACH;
    t->no_of_coach[3] = NO_OF_COACH;
    t->coach = (int **)malloc(sizeof(int *) * NO_OF_TRAVELLING_CLASSES);
    t->coach[0] = (int *)malloc(sizeof(int) * t->no_of_coach[0]);
    t->coach[1] = (int *)malloc(sizeof(int) * t->no_of_coach[1]);
    t->coach[2] = (int *)malloc(sizeof(int) * t->no_of_coach[2]);
    t->coach[3] = (int *)malloc(sizeof(int) * t->no_of_coach[3]);
    int i,j;
    for (i=0; i<4; i++)
    {
        for (j=0; j<t->no_of_coach[i]; j++)
        {
            (t->coach[i])[j] = NO_OF_SEATS_PER_COACH;
        }
    }
    t->left = NULL;
    t->right = NULL;
    t->down = NULL;
    t->height = 0;
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//To get balance value, height for avl insertion in trees of passenger, trains, date
int max(int a, int b)
{
    int x = b;
    if (a>b)
    {
        x = a;
    }
    return x;
}

int height_Date(Date *N)
{
    int k = 0;
    if (N == NULL)
    {
        k = 0;
    }
    else
    {
        k = 1+max(height_Date(N->left), height_Date(N->right));
    }
    return k;
}

int height_Train(Train *N)
{
    int k = 0;
    if (N == NULL)
    {
        k = 0;
    }
    else
    {
        k = 1+max(height_Train(N->left), height_Train(N->right));
    }
    return k;
}

int height_Passenger(Passenger *N)
{
    int k = 0;
    if (N == NULL)
    {
        k = 0;
    }
    else
    {
        k = 1+max(height_Passenger(N->left), height_Passenger(N->right));
    }
    return k;
}

Date *rightRotate_Date(Date *y)
{
    Date *x = y->left;
    Date *T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = height_Date(y);
    x->height = height_Date(x);
    return x;
}

Train *rightRotate_Train(Train *y)
{
    Train *x = y->left;
    Train *T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = height_Train(y);
    x->height = height_Train(x);
    return x;
}

Passenger *rightRotate_Passenger(Passenger *y)
{
    Passenger *x = y->left;
    Passenger *T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = height_Passenger(y);
    x->height = height_Passenger(x);
    return x;
}

Date *leftRotate_Date(Date * x)
{
    Date * y = x->right;
    Date * T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = height_Date(x);
    y->height = height_Date(y);
    return y;
}

Train *leftRotate_Train(Train * x)
{
    Train * y = x->right;
    Train * T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = height_Train(x);
    y->height = height_Train(y);
    return y;
}

Passenger *leftRotate_Passenger(Passenger * x)
{
    Passenger * y = x->right;
    Passenger * T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = height_Passenger(x);
    y->height = height_Passenger(y);
    return y;
}

int getBalance_Date(Date *N)
{
    int k;
    if (N == NULL)
    {
        k = 0;
    }
    else
    {
        k = height_Date(N->left) - height_Date(N->right);
    }
    return k;
}

int getBalance_Train(Train *N)
{
    int k;
    if (N == NULL)
    {
        k = 0;
    }
    else
    {
        k = height_Train(N->left) - height_Train(N->right);
    }
    return k;
}

int getBalance_Passenger(Passenger *N)
{
    int k;
    if (N == NULL)
    {
        k = 0;
    }
    else
    {
        k = height_Passenger(N->left) - height_Passenger(N->right);
    }
    return k;
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//To insert in Date tree, function returns root node
Date* insert_Date(Date * node, int d1, int m1, int y1, Date ** p)
{
    if (node == NULL)
    {
        Date * n = (Date *)malloc(sizeof(Date));
        initialise_Date(n,d1,m1,y1);
        *p = n;                 //to record the added date
        return n;
    }
    int key = days_equivalents(d1,m1,y1);
    if (key < days_equivalents(node->date,node->month,node->year))
    {
        node->left  = insert_Date(node->left, d1, m1, y1, p);
    }
    else if (key > days_equivalents(node->date,node->month,node->year))
    {
        node->right = insert_Date(node->right, d1, m1, y1, p);
    }
    else
    {
        *p = node;                  //to record the added date
        return node;
    }
    int s = sizeof(Date);
    node->height = height_Date(node);
    int balance = getBalance_Date(node);
    if (balance > 1 && key < days_equivalents(node->left->date,node->left->month,node->left->year))
    {
        return rightRotate_Date(node);
    }
    else if (balance < -1 && key > days_equivalents(node->right->date,node->right->month,node->right->year))
    {
        return leftRotate_Date(node);
    }
    else if (balance > 1 && key > days_equivalents(node->left->date,node->left->month,node->left->year))
    {
        node->left =  leftRotate_Date(node->left);
        return rightRotate_Date(node);
    }
    if (balance < -1 && key < days_equivalents(node->right->date,node->right->month,node->right->year))
    {
        node->right = rightRotate_Date(node->right);
        return leftRotate_Date(node);
    }
    return node;
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//To insert Train node in train tree
Train* insert_Train(Train* node, int key)
{
    if (node == NULL)
    {
        Train * n = (Train *)malloc(sizeof(Train));
        n->height = 0;
        n->train_id = key;
        n->left = NULL;
        n->right = NULL;
        initialise_train(&n);
        return n;
    }
    if (key < node->train_id)
    {
        node->left  = insert_Train(node->left, key);
    }
    else if (key > node->train_id)
    {
        node->right = insert_Train(node->right, key);
    }
    else
    {
        return node;
    }
    node->height = height_Train(node);
    int balance = getBalance_Train(node);
    if (balance > 1 && key < node->left->train_id)
    {
        return rightRotate_Train(node);
    }
    else if (balance < -1 && key > node->right->train_id)
    {
        return leftRotate_Train(node);
    }
    else if (balance > 1 && key > node->left->train_id)
    {
        node->left =  leftRotate_Train(node->left);
        return rightRotate_Train(node);
    }
    if (balance < -1 && key < node->right->train_id)
    {
        node->right = rightRotate_Train(node->right);
        return leftRotate_Train(node);
    }
    return node;
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//To insert passenger in passenger tree
Passenger * insert_Passenger(Passenger * node, int p_id, int bogie_no, int seat_no, Passenger ** ps_curr)
{
    if (node == NULL)
    {
        Passenger * n = (Passenger *)malloc(sizeof(Passenger));
        n->height = 0;
        n->passenger_id = p_id;
        n->left = NULL;
        n->bogie_no = bogie_no;
        n->seat = seat_no;
        n->right = NULL;
        *ps_curr = n;
        return n;
    }
    int key = (bogie_no * NO_OF_SEATS_PER_COACH) + seat_no;
    if (key < (node->bogie_no * NO_OF_SEATS_PER_COACH) + node->seat)
    {
        node->left  = insert_Passenger(node->left, p_id, bogie_no, seat_no,ps_curr);
    }
    else if (key > (node->bogie_no * NO_OF_SEATS_PER_COACH) + node->seat)
    {
        node->right = insert_Passenger(node->right, p_id, bogie_no, seat_no,ps_curr);
    }
    else
    {
        *ps_curr = node;
        return node;
    }
    node->height = height_Passenger(node);
    int balance = getBalance_Passenger(node);
    if (balance > 1 && key < (node->left->bogie_no * NO_OF_SEATS_PER_COACH) + node->left->seat)
    {
        return rightRotate_Passenger(node);
    }
    else if (balance < -1 && key > (node->right->bogie_no * NO_OF_SEATS_PER_COACH) + node->right->seat)
    {
        return leftRotate_Passenger(node);
    }
    else if (balance > 1 && key >(node->left->bogie_no * NO_OF_SEATS_PER_COACH) + node->left->seat)
    {
        node->left =  leftRotate_Passenger(node->left);
        return rightRotate_Passenger(node);
    }
    else if (balance < -1 && key < (node->right->bogie_no * NO_OF_SEATS_PER_COACH) + node->right->seat)
    {
        node->right = rightRotate_Passenger(node->right);
        return leftRotate_Passenger(node);
    }
    return node;
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//For Date tree traversal
void preOrder_Date(Date * d)
{
    if(d != NULL)
    {
        printf("%d %d %d, ", d->date, d->month, d->year);
        preOrder_Date(d->left);
        preOrder_Date(d->right);
    }
}
//For Train tree traversal
void preOrder_Train(Train * d)
{
    if(d != NULL)
    {
        printf("%d ", d->train_id);
        preOrder_Train(d->left);
        preOrder_Train(d->right);
    }
}

void print_space()
{
    printf("\n\n");
}
//For passenger tree traversal
void preOrder_Passenger(Passenger * d)
{
    if(d != NULL)
    {
        printf("%d ", d->passenger_id);
        preOrder_Passenger(d->left);
        preOrder_Passenger(d->right);
    }
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//To initialise date node
void initialise_Date(Date * D, int d, int m, int y)
{
    Date * p;
    p = D;
    p->date = d;
    p->month = m;
    p->year = y;
    p->left = NULL;
    p->right = NULL;
    p->down = NULL;
    p->height = 0;
    Train * t;
    p->down = insert_Train(p->down,12139);
    p->down = insert_Train(p->down,12140);
    p->down = insert_Train(p->down,14439);
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//SEARCH FOR A PARTICULAR DATE
Date* search_Date(Date * root, int d, int m, int y)
{
    Date * k;
    int key = days_equivalents(d,m,y);
    if (root == NULL)
    {
        k = NULL;
    }
    else if (days_equivalents(root->date, root->month, root->year) == key)
    {
        k = root;
    }
    else if (days_equivalents(root->date, root->month, root->year) < key)
    {
        k = search_Date(root->right, d, m, y);
    }
    else
    {
        k = search_Date(root->left, d, m, y);
    }
    return k;
}
//SEARCH FOR A PARTICULAR TRAIN WITH GIVEN TRAIN_ID
Train * search_Train(Train * root, int key)
{
    Train * k;
    if (root == NULL)
    {
        k = NULL;
    }
    else if (root->train_id == key)
    {
        k = root;
    }
    else if (root->train_id < key)
    {
        k = search_Train(root->right, key);
    }
    else
    {
        k = search_Train(root->left, key);
    }
    return k;
}

//SEARCH FOR PASSENGER FOR A GIVEN PASSENGER_ID
Passenger * search_Passenger(Passenger * root, int key)
{
    Passenger * k;
    if (root == NULL)
    {
        k = NULL;
    }
    else if (root->passenger_id == key)
    {
        k = root;
    }
    else if (root->passenger_id < key)
    {
        k = search_Passenger(root->right, key);
    }
    else
    {
        k = search_Passenger(root->left, key);
    }
    return k;
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//To take input of all passenger details
void input(int * no_of_request, char *** names, int ** p_id, char *** boarding_train, char *** b_station, char *** t_class, char *** d_station, int ** t_id, int ** upgrade, int * dt, int *mt ,int *yr)
{
    //Ask input from user
    //ASSUMPTION: People making reservation together go from the same train on the same day where classes may be different
    int v=0,i,t_num,a,b,c;
    printf("Enter the date, month, year(of travel), train id and number of requests: ");
    scanf("%d %d %d %d %d",&a,&b,&c,&t_num,&v);
    *dt = a;
    *mt = b;
    *yr = c;
    *no_of_request = v;
    int no_of_requests = v;
    char **n = (char **)malloc(sizeof(char *)*no_of_requests);
    int *p = (int *)malloc(sizeof(int) * no_of_requests);
    int *t = (int *)malloc(sizeof(int) * no_of_requests);
    char **boarding = (char **)malloc(sizeof(char *)*no_of_requests);
    char **b_stat = (char **)malloc(sizeof(char *)* no_of_requests);
    char **d_stat = (char **)malloc(sizeof(char *)* no_of_requests);
    char **t_clas = (char **)malloc(sizeof(char *)* no_of_requests);
    int *upgrd = (int *)malloc(sizeof(int) * no_of_requests);
    for (i=0; i<no_of_requests; i++)
    {
        n[i] = (char *)malloc(sizeof(char ) * 20);
        boarding[i] = (char *)malloc(sizeof(char) * 20);
        b_stat[i] = (char *)malloc(sizeof(char ) * 20);
        d_stat[i] = (char *)malloc(sizeof(char ) * 20);
        t_clas[i] = (char *)malloc(sizeof(char ) * 20);
        printf("Enter the name of the person, Passenger id, boarding train name, boarding station, destination station, travelling class(AC1/AC2/AC3/Sleeper), upgrade required(1/0): ");
        scanf("%s %d %s %s %s %s %d",n[i],&p[i],boarding[i],b_stat[i],d_stat[i],t_clas[i],&upgrd[i]);
        t[i] = t_num;
    }
    *names = n;
    *p_id = p;
    *t_id = t;
    *boarding_train = boarding;
    *b_station = b_stat;
    *d_station = d_stat;
    *upgrade = upgrd;
    *t_class = t_clas;
}

//FUNCTION TO FIND INDICES FROM TRAVELLING CLASS INPUT by using the last character of input: AC1,AC2,AC3,Sleeper
int find_unique(char a)
{
    if (a=='3')
    {
        return 3;
    }
    else if (a=='2')
    {
        return 2;
    }
    else if (a=='1')
    {
        return 1;
    }
    return 4;
}

void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

void swap1(void **m,void **n, int Size){
    if (Size == 1)
    {
        char ** a = (char **)m;
        char ** b = (char **)n;
        char *temp;
        temp = *a;
        *a = *b;
        *b = temp;
    }
    else if (Size == sizeof(Train))
    {
        Train ** a = (Train **)m;
        Train ** b = (Train **)n;
        Train * temp;
        temp = *a;
        *a = *b;
        *b = temp;
    }
    else if (Size == sizeof(Date))
    {
        Date ** a = (Date **)m;
        Date ** b = (Date **)n;
        Date * temp;
        temp = *a;
        *a = *b;
        *b = temp;
    }
    else if (Size == sizeof(Passenger))
    {
        Passenger ** a = (Passenger **)m;
        Passenger ** b = (Passenger **)n;
        Passenger * temp;
        temp = *a;
        *a = *b;
        *b = temp;
    }
}

int partition (void ** r, int * arr, int low, int high, int Size)
{
    if (Size == 1)
    {
        char ** s = (char **)r;
        int pivot = arr[high];
        int i = (low - 1);
        for (int j = low; j <= high - 1; j++)
        {
            if (arr[j] < pivot)
            {
                i++;
                swap(&arr[i], &arr[j]);
                swap1(&(((void **)s)[i]),&(((void **)s)[j]),Size);
            }
        }
        swap(&arr[i + 1], &arr[high]);
        swap1(&(((void **)s)[i+1]),&(((void **)s)[high]),Size);
        return (i + 1);
    }
    else if (Size == sizeof(Train))
    {
        Train ** s = (Train **)r;
        int pivot = arr[high];
        int i = (low - 1);
        for (int j = low; j <= high - 1; j++)
        {
            if (arr[j] < pivot)
            {
                i++;
                swap(&arr[i], &arr[j]);
                swap1(&(((void **)s)[i]),&(((void **)s)[j]),Size);
            }
        }
        swap(&arr[i + 1], &arr[high]);
        swap1(&(((void **)s)[i+1]),&(((void **)s)[high]),Size);
        return (i + 1);
    }
    else if (Size == sizeof(Date))
    {
        Date ** s = (Date **)r;
        int pivot = arr[high];
        int i = (low - 1);
        for (int j = low; j <= high - 1; j++)
        {
            if (arr[j] < pivot)
            {
                i++;
                swap(&arr[i], &arr[j]);
                swap1(&(((void **)s)[i]),&(((void **)s)[j]),Size);
            }
        }
        swap(&arr[i + 1], &arr[high]);
        swap1(&(((void **)s)[i+1]),&(((void **)s)[high]),Size);
        return (i + 1);
    }
    else if (Size == sizeof(Passenger))
    {
        Passenger ** s = (Passenger **)r;
        int pivot = arr[high];
        int i = (low - 1);
        for (int j = low; j <= high - 1; j++)
        {
            if (arr[j] < pivot)
            {
                i++;
                swap(&arr[i], &arr[j]);
                swap1(&(((void **)s)[i]),&(((void **)s)[j]),Size);
            }
        }
        swap(&arr[i + 1], &arr[high]);
        swap1(&(((void **)s)[i+1]),&(((void **)s)[high]),Size);
        return (i + 1);
    }
}
//To sort void ** r based on int * arr
void quickSort(void ** r, int * arr, int low, int high, int Size)
{
    if (low < high)
    {
        int pi = partition(r,arr, low, high,Size);
        quickSort(r,arr, low, pi - 1,Size);
        quickSort(r,arr, pi + 1, high,Size);
    }
}

int partition1 (int * arr1, int * arr, int low, int high)
{
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++)
    {
        if (arr[j] < pivot)
        {
            i++;
            swap(&arr[i], &arr[j]);
            swap(&arr1[i], &arr1[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    swap(&arr1[i + 1], &arr1[high]);
    return (i + 1);
}
//To sort int arr1 in sorted order of int arr
void quickSort1(int * arr1, int * arr, int low, int high)
{
    if (low < high)
    {
        int pi = partition1(arr1,arr, low, high);
        quickSort1(arr1,arr, low, pi - 1);
        quickSort1(arr1,arr, pi + 1, high);
    }
}
//FUNCTION TO INSERT THE PASSENGER DETAILS
void insert(Date ** d)
{
    //At a time insertion to only one train is allowed
    int no_of_request=0;
    char ** names;
    int * p_id;
    char** boarding_train;
    char** b_station;
    char** t_class;
    char ** d_station;
    int * t_id;
    int * upgrade;
    int dt;
    int mt;
    int yr;
    input(&no_of_request, &names, &p_id, &boarding_train, &b_station, &t_class, &d_station, &t_id, &upgrade, &dt, &mt, &yr);
    Date * p;
    *d = insert_Date(*d,dt,mt,yr,&p);     //Will return pointer to THAT date if present or will insert a date
    Train * t;
    t = search_Train(p->down, t_id[0]);        //Will search amongst the train running on the given date
    if (t!=NULL)
    {
        Passenger * ps;
        ps = t->down;
        int j,i=0;
        j=0;
        //Sort the fields in the order of travelling class
        int * a = (int *)malloc(sizeof(int)*no_of_request);
        for (i=0; i<no_of_request; i++)
        {
            a[i] = find_unique(t_class[i][strlen(t_class[i])-1]);
        }
        quickSort((void **)names,a,0,no_of_request-1,1);
        for (i=0; i<no_of_request; i++)
        {
            a[i] = find_unique(t_class[i][strlen(t_class[i])-1]);
        }
        quickSort((void **)t_class,a,0,no_of_request-1,1);
        for (i=0; i<no_of_request; i++)
        {
            a[i] = find_unique(t_class[i][strlen(t_class[i])-1]);
        }
        quickSort((void **)b_station,a,0,no_of_request-1,1);
        for (i=0; i<no_of_request; i++)
        {
            a[i] = find_unique(t_class[i][strlen(t_class[i])-1]);
        }
        quickSort((void **)d_station,a,0,no_of_request-1,1);
        for (i=0; i<no_of_request; i++)
        {
            a[i] = find_unique(t_class[i][strlen(t_class[i])-1]);
        }
        quickSort1(p_id,a,0,no_of_request-1);
        int count[4] = {0};     //To store requests for each class: AC1,AC2,AC3,Sleeper
        for (i=0; i<no_of_request; i++)
        {
            count[find_unique(t_class[i][strlen(t_class[i])-1])-1]+=1;          //Count of requests for each travelling class with indices 0:AC1, 1:AC2, 2:AC3, 3:Sleeper
        }
        int val,req,k;
        int ct=0;
        for (i=0; i<4; i++)
        {
            printf("@%d ",count[i]);
            if (count[i]>0)
            {
                if (t->ALL_availability[i] > 0 && count[i] <= t->ALL_availability[i])
                {
                    val=0;
                    req=-1;
                    k=0;
                    while (k<t->no_of_coach[i] && val==0)           //Finding first bogie which can satisfy the request
                    {
                        if (t->coach[i][k] >= count[i])
                        {
                            req = k;
                            val = t->coach[i][k];
                        }
                        k++;
                    }
                    while (k<t->no_of_coach[i])                     //Finding best fit bogie which can satisfy the request
                    {
                        if (val > t->coach[i][k])
                        {
                            req = k;
                            val = t->coach[i][k];
                        }
                        k++;
                    }
                    if (val > 0)                      //Check if any matching bogie is found
                    {
                        while (count[i] > 0)            //Add passenger for all the requests made
                        {
                            count[i]-=1;
                            Passenger * pg;
                            t->down = insert_Passenger(t->down,p_id[ct], NO_OF_COACH*i + req + 1, NO_OF_SEATS_PER_COACH - val + 1, &pg);
                            pg->apply_date = dt;
                            pg->apply_month = mt;
                            pg->apply_year = yr;
                            strcpy(pg->boarding_station,b_station[ct]);
                            strcpy(pg->destination,d_station[ct]);
                            strcpy(pg->name,names[ct]);
                            pg->train_id = t_id[ct];
                            strcpy(pg->travelling_class,t_class[ct]);
                            pg->upgrade = upgrade[ct];
                            t->ALL_availability[i]-=1;
                            t->coach[i][req] -= 1;
                            ct++;
                            val--;
                        }
                    }
                    else                //val == 0 =>No single bogie can fulfill the request
                    {
                        k=0;
                        val = 0;
                        req = -1;
                        int y=0;
                        int sum,min=count[i],cttt=0;
                        while (k<t->no_of_coach[i] && t->coach[i][k] == 0)      //Eliminating bogies with zero vacant seats
                        {
                            y = k+1;
                            k++;
                        }
                        while (k<t->no_of_coach[i]-1)           //To find the index of bogie such that allocation from or after that bogie will require lesser bogies ie. more people can stay together
                        {
                            sum=0;
                            cttt=0;
                            y = k;
                            while (y<t->no_of_coach[i] && sum<count[i])
                            {
                                cttt++;
                                sum+=t->coach[i][y];
                                y++;
                            }
                            if (sum>=count[i] && min>cttt)
                            {
                                min = cttt;
                                req = k;
                            }
                            k++;
                        }
                        k=req;
                        int ctt = 0;
                        while (k<t->no_of_coach[i] && count[i] > 0)     //Allocate the Passenger seats to the bogie
                        {
                            if (t->coach[i][k] > 0)             //If the bogie is not full allocate from it
                            {
                                if (count[i] >= t->coach[i][k])
                                {
                                    while (t->coach[i][k] > 0)
                                    {
                                        count[i]-=1;
                                        Passenger * pg;
                                        t->down = insert_Passenger(t->down,p_id[ct], NO_OF_COACH*i + k + 1, NO_OF_SEATS_PER_COACH - t->coach[i][k], &pg);
                                        pg->apply_date = dt;
                                        pg->apply_month = mt;
                                        pg->apply_year = yr;
                                        strcpy(pg->boarding_station,b_station[ct]);
                                        strcpy(pg->destination,d_station[ct]);
                                        strcpy(pg->name,names[ct]);
                                        pg->passenger_id = p_id[ct];
                                        pg->train_id = t_id[ct];
                                        strcpy(pg->travelling_class,t_class[ct]);
                                        pg->upgrade = upgrade[ct];
                                        t->ALL_availability[i]-=1;
                                        t->coach[i][k] -= 1;
                                        ct++;
                                    }
                                }
                                else
                                {
                                    while (count[i] > 0)
                                    {
                                        count[i]-=1;
                                        Passenger * pg;
                                        t->down = insert_Passenger(t->down,p_id[ct], NO_OF_COACH*i + k + 1, NO_OF_SEATS_PER_COACH - t->coach[i][k], &pg);
                                        pg->apply_date = dt;
                                        pg->apply_month = mt;
                                        pg->apply_year = yr;
                                        strcpy(pg->boarding_station,b_station[ct]);
                                        strcpy(pg->destination,d_station[ct]);
                                        strcpy(pg->name,names[ct]);
                                        pg->train_id = t_id[ct];
                                        strcpy(pg->travelling_class,t_class[ct]);
                                        pg->upgrade = upgrade[ct];
                                        t->ALL_availability[i]-=1;
                                        t->coach[i][k] -= 1;
                                        ct++;
                                    }
                                }
                            }
                            ctt++;
                            k++;
                        }
                    }
                }
                else if (t->ALL_availability[i] > 0)        //If not all Passengers can be allocated do partial allocation
                {
                    int y = count[i];
                    int ctt = 0;
                    k=0;
                    while (k<t->no_of_coach[i])
                    {
                        if (t->coach[i][k] > 0)
                        {
                            while (t->coach[i][k] > 0)
                            {
                                count[i]-=1;
                                Passenger * pg;
                                t->down = insert_Passenger(t->down,p_id[ct], NO_OF_COACH*i + k + 1, NO_OF_SEATS_PER_COACH - t->coach[i][k], &pg);
                                pg->apply_date = dt;
                                pg->apply_month = mt;
                                pg->apply_year = yr;
                                strcpy(pg->boarding_station,b_station[ct]);
                                strcpy(pg->destination,d_station[ct]);
                                strcpy(pg->name,names[ct]);
                                pg->train_id = t_id[ct];
                                strcpy(pg->travelling_class,t_class[ct]);
                                pg->upgrade = upgrade[ct];
                                t->ALL_availability[i]-=1;
                                t->coach[i][k] -= 1;
                                ct++;
                                ctt++;
                            }
                        }
                        k++;
                    }
                    ct = ct - ctt + y;
                    printf("Partially Registered");
                }
                else
                {
                    printf("Reservation Failed\n");
                }
            }
        }
        free(a);
    }
    free(p_id);
    int i;
    for (i=0; i<no_of_request; i++)
    {
        free(names[i]);
        free(boarding_train[i]);
        free(b_station[i]);
        free(d_station[i]);
        free(t_class[i]);
    }
    free(names);
    free(boarding_train);
    free(b_station);
    free(d_station);
    free(t_class);
    free(t_id);
    free(upgrade);
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//To get smallest value node
Passenger * minValueNode(Passenger* node)
{
    Passenger* cur = node;
    while (cur->left != NULL)
    {
        cur = cur->left;
    }
    return cur;
}
//To delete a node
Passenger* deleteNode(Passenger * root, int key, int * note)
{
    if (root == NULL)
    {
        return root;
    }
    else if ( key < root->passenger_id )
    {
        root->left = deleteNode(root->left, key, note);
    }
    else if( key > root->passenger_id )
    {
        root->right = deleteNode(root->right, key, note);
    }
    else
    {
        (*note)++;                  //To mark deletion of a node
        if( (root->left == NULL) || (root->right == NULL) )
        {
            Passenger *temp;
            if (root->left != NULL)
            {
                temp = root->left;
            }
            else
            {
                temp = root->right;
            }
            if (temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            else
            {
                *root = *temp;
            }
            free(temp);
        }
        else
        {
            Passenger* temp = minValueNode(root->right);
            root->passenger_id = temp->passenger_id;
            root->right = deleteNode(root->right, temp->passenger_id, note);
        }
    }
    if (root == NULL)
    {
        return root;
    }
    root->height = 1 + max(height_Passenger(root->left),height_Passenger(root->right));
    int balance = getBalance_Passenger(root);
    if (balance > 1 && getBalance_Passenger(root->left) >= 0)
    {
        return rightRotate_Passenger(root);
    }
    else if (balance > 1 && getBalance_Date(root->left) < 0)
    {
        root->left =  leftRotate_Passenger(root->left);
        return rightRotate_Passenger(root);
    }
    else if (balance < -1 && getBalance_Passenger(root->right) <= 0)
    {
        return leftRotate_Passenger(root);
    }
    else if (balance < -1 && getBalance_Passenger(root->right) > 0)
    {
        root->right = rightRotate_Passenger(root->right);
        return leftRotate_Passenger(root);
    }
    return root;
}
//To traverse all Trains to delete passenger with given passenger id
void traverse_Train4(Train * d, int p_id, int * ct)
{
    if (d!=NULL)
    {
        d->down = deleteNode(d->down,p_id,ct);
        traverse_Train4(d->left,p_id,ct);
        traverse_Train4(d->right,p_id,ct);
    }
}

void traverse_Date4(Date * d, int p_id, int *ct)
{
    if (d!=NULL)
    {
        traverse_Train4(d->down,p_id,ct);
        traverse_Date4(d->left,p_id,ct);
        traverse_Date4(d->right,p_id,ct);
    }
}
//To delete all reservations of that particular passenger id
void delete_reservation(Date * d, int p_id)
{
    int c = 0;      //To check if any deletion is done
    traverse_Date4(d,p_id,&c);
    if (c==0)
    {
        printf("NO RESERVATION FROM PASSENGER ID:%d FOUND.... CANCELLATION FAILED\n",p_id);
    }
    else
    {
        printf("SUCCESSFUL CANCELLATION FOR PASSENGER ID:%d \n",p_id);
    }
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//To define structures for linked list to create a 2-D linked list
typedef struct LL1      //Stores pointer to passenger and date
{                       //To store the passenger info
    Passenger * p;
    Date * dt;
    struct LL1 * next;
}Store_pass;

typedef struct LL       //Stores destination and train id
{
    char * dest;
    int t_id;
    struct LL * next;
    struct Store_pass * down;
}Store_destination;

//To search in linked list Store_destination and if not found create a node with given destination and train id
//A 2-d linked list is needed as for each destination and train id number of same passengers will be different
Store_destination * search_Or_Insert_LL(Store_destination ** pt, char * dest, int t_id)
{
    Store_destination * l;
    Store_destination *prev;
    Store_destination * y = NULL;
    l = *pt;
    prev = NULL;
    while (l!=NULL && y==NULL)
    {
        if (strcmp(l->dest,dest) == 0 && l->t_id == t_id)
        {
            y = l;
        }
        prev = l;
        l = l->next;
    }
    if (y==NULL)
    {
        Store_destination * n = (Store_destination *)malloc(sizeof(Store_destination));
        n->t_id = t_id;
        n->dest = (char *)malloc(20);
        strcpy(n->dest,dest);
        n->down = NULL;
        n->next = NULL;
        if (prev == NULL)
        {
            *pt = n;
        }
        else
        {
            prev->next = n;
        }
        y = n;
    }
    return y;
}
//To insert passenger at linked list
void insert_in_Store_pass(Store_pass ** pt, Date * dt, Passenger * ps)
{
    Store_pass * n = (Store_pass *)malloc(sizeof(Store_pass));
    n->dt = dt;
    n->next = NULL;
    n->p = ps;
    if (*pt == NULL)
    {
        *pt = n;
    }
    else
    {
        Store_pass * l , *prev;
        prev = NULL;
        l = *pt;
        while (l!=NULL)
        {
            prev = l;
            l = l->next;
        }
        prev->next = n;
    }
}

void traverse_Passenger(Passenger * d, Store_destination ** l, Date * p, int t_id)
{
    if (d!=NULL)
    {
        Store_destination * ptr = search_Or_Insert_LL(l,d->destination,t_id);       //To search for LL node with given destination and train_id
        Store_destination * n = *l;
        Store_pass * nptr;
        nptr = ptr->down;
        insert_in_Store_pass(&nptr,p,d);                //To insert at the end of the linked list associated with the searched node
        ptr->down = nptr;
        traverse_Passenger(d->left,l,p,t_id);
        traverse_Passenger(d->right,l,p,t_id);
    }
}

void traverse_Train(Train * d, Store_destination ** l, Date * dt)
{
    if (d!=NULL)
    {
        traverse_Passenger(d->down,l,dt,d->train_id);
        traverse_Train(d->left,l,dt);
        traverse_Train(d->right,l,dt);
    }
}

void traverse_Date(Date * d, Store_destination ** l)
{
    if (d!=NULL)
    {
        traverse_Train(d->down,l,d);
        traverse_Date(d->left,l);
        traverse_Date(d->right,l);
    }
}
//FUNCTION TO CREATE A LIST WHICH STORES PASSENGERS WITH SAME DESTINATION AND SAME TRAIN ID
void get_list_Destination(Date * d)
{
    Store_destination * temp = NULL;
    traverse_Date(d,&temp);
    Store_pass * p;
    Store_destination * a;
    Store_pass * b;
    while (temp!=NULL)
    {
        a=temp;
        printf("%d %s: \n",temp->t_id, temp->dest);     //Train id and destination
        p = temp->down;
        while (p!=NULL)
        {
            b = p;
            printf("On date:%d / %d / %d Passenger id : %d, ",p->dt->date,p->dt->month,p->dt->year,p->p->passenger_id);
            p = p->next;
            free(b);
        }
        printf("have same destination and train id");
        printf("\n");
        temp = temp->next;
        free(a->dest);
        free(a);
    }
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//To traverse thru Passenger tree and print destination
void traverse_Passenger1(Passenger * d, int p_id)
{
    if (d!=NULL)
    {
        if (d->passenger_id == p_id)
        {
            printf("%s, ",d->destination);
        }
        traverse_Passenger1(d->left, p_id);
        traverse_Passenger1(d->right, p_id);
    }
}

//To traverse thru Train tree
void traverse_Train1(Train * d, int p_id)
{
    if (d!=NULL)
    {
        traverse_Passenger1(d->down, p_id);
        traverse_Train1(d->left, p_id);
        traverse_Train1(d->right, p_id);
    }
}

void traverse_Date1(Date * d, int p_id)
{
    if (d!=NULL)
    {
        traverse_Date1(d->left, p_id);
        printf("\nOn Date: %d / %d / %d : ",d->date, d->month, d->year);
        traverse_Train1(d->down, p_id);
        traverse_Date1(d->right, p_id);
    }
}

//FUNCTION TO DISPLAY PASSENGER WITH PASSENGER ID P_ID DATE-WISE
void Sort_by_Travel_Date(Date * d, int p_id)
{
    traverse_Date1(d,p_id);         //Date tree are already sorted
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//To count the number of trains
void traverse_Train_count(Train * d, int * ct)
{
    if (d!=NULL)
    {
        (*ct)++;
        traverse_Train_count(d->left,ct);
        traverse_Train_count(d->right,ct);
    }
}

void traverse_Date_count(Date * d, int * ct)
{
    if (d!=NULL)
    {
        traverse_Train_count(d->down,ct);
        traverse_Date_count(d->left,ct);
        traverse_Date_count(d->right,ct);
    }
}
//To put values in arrays:
void traverse_Train2(Train * d, Train ** arr, Date ** brr, int * seat_ct, Date * dt, int * val)
{
    if (d!=NULL)
    {
        arr[*val] = d;
        seat_ct[*val] = Train_capacity - (d->ALL_availability[0] + d->ALL_availability[1] + d->ALL_availability[2] + d->ALL_availability[3]);
        brr[(*val)++] = dt;
        traverse_Train2(d->left,arr,brr,seat_ct, dt, val);
        traverse_Train2(d->right,arr,brr,seat_ct, dt, val);
    }
}

void traverse_Date2(Date * d, Train ** arr, Date ** brr, int * seat_ct, int * val)
{
    if (d!=NULL)
    {
        traverse_Train2(d->down,arr,brr,seat_ct,d,val);
        traverse_Date2(d->left,arr,brr,seat_ct,val);
        traverse_Date2(d->right,arr,brr,seat_ct,val);
    }
}
//TO DISPLAY DESTINATION FOR A PASSENGER ID AS PER DATES
void Sort_Train(Date * d)
{
    printf("Train number and travel Dates in sorted order of number of Passengers:\n");
    int i;
    int ct = 0;
    int val = 0;
    traverse_Date_count(d,&ct);     //To store total number of trains in all dates
    Train ** arr = (Train **)malloc(sizeof(Train*)*ct);     //Array of pointers to train
    Date ** brr = (Date **)malloc(sizeof(Date*)*ct);        //Array of pointers to date
    int * seat_ct1 = (int *)malloc(sizeof(Train)*ct);       //To count number of passengers
    int * seat_ct2 = (int *)malloc(sizeof(Train)*ct);       //to count number of passengers
    traverse_Date2(d,arr,brr,seat_ct1,&val);
    for (i=0; i<ct; i++)
    {
        seat_ct2[i] = seat_ct1[i];
    }
    //To sort trains and dates pointers with same field to maintain there one -to -one relation
    quickSort((void **)arr,seat_ct1,0,ct-1,sizeof(Train));      //To array of pointers to train arr in sorted order of number of passengers in the train
    quickSort((void **)brr,seat_ct2,0,ct-1,sizeof(Date));       //To array of pointers to train arr in sorted order of number of passengers in the train
    for (i=0; i<ct; i++)
    {
        printf("Train id: %d \t Number of passenger: %d \t Date:%d / %d / %d \n",arr[i]->train_id,seat_ct1[i],brr[i]->date,brr[i]->month,brr[i]->year);
    }
    free(arr);
    free(brr);
    free(seat_ct1);
    free(seat_ct2);
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//To traverse trains and print train if in the range
void traverse_Train3(Train * d, int from_t_id, int to_t_id)
{
    if (d!=NULL)
    {
        if (d->train_id >= from_t_id && d->train_id < to_t_id)
        {
            printf("%d, ",d->train_id);
        }
        traverse_Train3(d->left, from_t_id, to_t_id);
        traverse_Train3(d->right, from_t_id, to_t_id);
    }
}

void traverse_Date3(Date * d, int from_t_id, int to_t_id)
{
    if (d!=NULL)
    {
        printf("\nOn DATE: %d / %d / %d: ",d->date, d->month, d->year);
        traverse_Train3(d->down, from_t_id, to_t_id);
        traverse_Date3(d->left, from_t_id, to_t_id);
        traverse_Date3(d->right, from_t_id, to_t_id);
    }
}
//TO PRINT TRAIN ID IF FALLS IN THE RANGE
void Range_Search(Date *d, int from_t_id, int to_t_id)
{
    printf("RANGE SEARCH FROM %d TO %d:\n",from_t_id,to_t_id);
    traverse_Date3(d,from_t_id,to_t_id);
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
void put_random_nodes(Date ** D)
{
    Date * dt;
    *D = insert_Date(*D,1,5,2021,&dt);
    *D = insert_Date(*D,1,4,2021,&dt);
    *D = insert_Date(*D,1,2,2021,&dt);
    Date * d = *D;
    Passenger * p;
    Train * t = (search_Train(d->right->down,12140));
    t->down = insert_Passenger(t->down, 1, 2, 1, &p);
    p->train_id = 12140;
    p->apply_date = 1;
    p->apply_month = 11;
    p->apply_year = 2020;
    strcpy(p->boarding_station,"Nagpur");
    strcpy(p->name,"Name1");
    strcpy(p->travelling_class,"AC1");
    strcpy(p->destination,"Mumbai");
    p->upgrade = 0;
    t->ALL_availability[0]-=1;
    t->coach[0][1]-=1;

    t = (search_Train(d->right->down,12140));
    t->down = insert_Passenger(t->down, 2, 4, 1, &p);
    p->train_id = 12140;
    p->apply_date = 1;
    p->apply_month = 10;
    p->apply_year = 2020;
    strcpy(p->destination,"Mumbai");
    strcpy(p->boarding_station,"Nagpur");
    strcpy(p->name,"Name2");
    strcpy(p->travelling_class,"AC2");
    p->upgrade = 0;
    t->ALL_availability[1]-=1;
    t->coach[1][0]-=1;

    t = (search_Train(d->right->down,12140));
    t->down = insert_Passenger(t->down, 3, 5, 1, &p);
    p->train_id = 12140;
    p->apply_date = 4;
    p->apply_month = 11;
    p->apply_year = 2020;
    strcpy(p->destination,"Mumbai");
    strcpy(p->boarding_station,"Nagpur");
    strcpy(p->name,"Name3");
    strcpy(p->travelling_class,"AC2");
    p->upgrade = 1;
    t->ALL_availability[1]-=1;
    t->coach[1][2]-=1;

    t = (search_Train(d->right->down,12140));
    t->down = insert_Passenger(t->down, 4, 7, 1, &p);
    p->train_id = 12140;
    p->apply_date = 1;
    p->apply_month = 12;
    p->apply_year = 2020;
    strcpy(p->destination,"Mumbai");
    strcpy(p->boarding_station,"Nagpur");
    strcpy(p->name,"Name4");
    strcpy(p->travelling_class,"AC3");
    p->upgrade = 1;
    t->ALL_availability[2]-=1;
    t->coach[2][0]-=1;

    t = (search_Train(d->right->down,12140));
    t->down = insert_Passenger(t->down, 5, 5, 2, &p);
    p->train_id = 12140;
    p->apply_date = 15;
    p->apply_month = 11;
    p->apply_year = 2020;
    strcpy(p->destination,"Mumbai");
    strcpy(p->boarding_station,"Nagpur");
    strcpy(p->name,"Name5");
    strcpy(p->travelling_class,"AC2");
    p->upgrade = 1;
    t->ALL_availability[1]-=1;
    t->coach[1][1]-=1;

    t = (search_Train(d->left->down,14439));
    t->down = insert_Passenger(t->down, 6, 3, 1, &p);
    p->train_id = 14439;
    p->apply_date = 18;
    p->apply_month = 11;
    p->apply_year = 2020;
    strcpy(p->destination,"Delhi");
    strcpy(p->boarding_station,"Nagpur");
    strcpy(p->name,"Name6");
    strcpy(p->travelling_class,"AC1");
    p->upgrade = 0;
    t->ALL_availability[0]-=1;
    t->coach[0][2]-=1;

    t = (search_Train(d->down,12139));
    t->down = insert_Passenger(t->down, 1, 1, 1, &p);
    p->train_id = 12139;
    strcpy(p->destination,"Bhopal");
    strcpy(p->boarding_station,"Nagpur");
    strcpy(p->name,"Name1");
    strcpy(p->travelling_class,"AC1");
    p->upgrade = 1;
    p->apply_date = 21;
    p->apply_month = 12;
    p->apply_year = 2020;
    p->upgrade = 1;
    t->ALL_availability[0]-=1;
    t->coach[0][0]-=1;

    t = (search_Train(d->right->down,14439));
    t->down = insert_Passenger(t->down, 2, 2, 1, &p);
    p->train_id = 14439;
    strcpy(p->destination,"Delhi");
    strcpy(p->boarding_station,"Nagpur");
    strcpy(p->name,"Name2");
    strcpy(p->travelling_class,"AC1");
    p->apply_date = 9;
    p->apply_month = 5;
    p->apply_year = 2020;
    t->ALL_availability[0]-=1;
    t->coach[0][1]-=1;
}
/*--------------------------------------------------------------------------------------------------------------------------------------*/
//For printing all the nodes of date, train, passenger to check the results
void print_all_Passenger(Passenger * p)
{
    if (p!=NULL)
    {
        print_all_Passenger(p->left);
        printf("%d (Bogie: %d, Seat: %d), ",p->passenger_id,p->bogie_no,p->seat);
        print_all_Passenger(p->right);
    }
}

void print_all_Train(Train * t)
{
    if (t!=NULL)
    {
        printf("\n%d: ",t->train_id);
        print_all_Passenger(t->down);
        print_all_Train(t->left);
        print_all_Train(t->right);
    }
}

void print_all_Date(Date * d)
{
    if (d!=NULL)
    {
        printf("\n\n%d / %d / %d :",d->date,d->month,d->year);
        print_all_Train(d->down);
        print_all_Date(d->left);
        print_all_Date(d->right);
    }
}

void add_space()
{
    printf("\n\n");
}

int main()
{
    printf("*****************Assumption: there are 3 trains that run daily with train id: 12139,12140,14439****************\n");
    Date * d = NULL;
    put_random_nodes(&d);
    printf("\n<-----INSERTION------>\n");
    insert(&d);
    add_space();
    print_all_Date(d);
    int i,a,b,c;
    printf("\n<-----DELETION------>\n");
    printf("\nEnter pid to be deleted :");
    scanf("%d",&i);
    delete_reservation(d,i);
    add_space();
    print_all_Date(d);
    add_space();
    printf("\n<------GET LIST OF DESTINATION------>\n");
    get_list_Destination(d);
    add_space();
    printf("\n<------SORT_BY_TRAVEL_DATE------>\n");
    printf("\nEnter pid to find its destination sorted by travel date:");
    scanf("%d",&i);
    Sort_by_Travel_Date(d,i);
    add_space();
    printf("\n<------SORT_TRAIN------>\n");
    Sort_Train(d);
    add_space();
    printf("\n<------Range search------>\n");
    printf("\nEnter from and to train_ids for range search:");
    scanf("%d %d",&i,&a);
    Range_Search(d,i,a);
    print_all_Date(d);
    return 0;
}
