#include <bits/stdc++.h>
using namespace std;
#pragma GCC optimize("O2","unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#define pb push_back
#define pii pair<int,int>
#define F first
#define S second
#define mp make_pair
const int MAXN=20;

int dx[4]={0,1,0,-1};
int dy[4]={1,0,-1,0};
//         R D L U

#define TopLine i,j,i,j+1
#define DownLine i+1,j,i+1,j+1
#define LeftLine i,j,i+1,j
#define RightLine i,j+1,i+1,j+1

#define DownDownLine i+2,j,i+2,j+1
#define RightRightLine i,j+2,i+1,j+2

#define DownLeftLine i+1,j-1,i+1,j
#define DownRightLine i+1,j+1,i+1,j+2
#define RightTopLine i-1,j+1,i,j+1
#define RightDownLine i+1,j+1,i+2,j+1
#define TopLeftLine i,j-1,i,j
#define TopRightLine i,j+1,i,j+2
#define LeftTopLine i-1,j,i,j
#define LeftDownLine i+1,j,i+2,j

int Map[MAXN][MAXN];
int usedEdgeCnt[MAXN][MAXN];
bool usedNode[MAXN][MAXN];
//Edge[x1][x2][y1][y2] means (x1,y1)->(x2,y2)
bool linkedEdge[MAXN][MAXN][MAXN][MAXN];  
bool bannedEdge[MAXN][MAXN][MAXN][MAXN];

int n,m,Time;
pii StartPoint,prePos,curPos;

typedef struct cmd{
    int time,type,x1,y1,x2,y2;
}cmd;
stack<cmd> command; //restore modified edge

bool CheckBannedEdge(int x1,int y1,int x2,int y2){  //edge has been banned or not
    return bannedEdge[x1][y1][x2][y2] || bannedEdge[x2][y2][x1][y1];
}
bool CheckLinkedEdge(int x1,int y1,int x2,int y2){
    return linkedEdge[x1][y1][x2][y2] || linkedEdge[x2][y2][x1][y1];
}

bool goRight(int x,int y){ 
    if(CheckBannedEdge(x,y,x,y+1)) return false;
    if(CheckLinkedEdge(x,y,x,y+1)) return true;
    if(mp(x,y+1)==curPos) return true;  //.-(go right)->.(final point)--.--.--.......
    return !usedNode[x][y+1];
}
bool goLeft(int x,int y){
    if(CheckBannedEdge(x,y,x,y-1)) return false;
    if(CheckLinkedEdge(x,y,x,y-1)) return true;
    if(mp(x,y-1)==curPos) return true;
    return !usedNode[x][y-1];
}
bool goUp(int x,int y){
    if(CheckBannedEdge(x,y,x-1,y)) return false;
    if(CheckLinkedEdge(x,y,x-1,y)) return true;
    if(mp(x-1,y)==curPos) return true;
    return !usedNode[x-1][y];
}
bool goDown(int x,int y){
    if(CheckBannedEdge(x,y,x+1,y)) return false;
    if(CheckLinkedEdge(x,y,x+1,y)) return true;
    if(mp(x+1,y)==curPos) return true;
    return !usedNode[x+1][y];
}

bool BanEdge(int x1,int y1,int x2,int y2){ 
    if(bannedEdge[x1][y1][x2][y2]) return false;  //banned already
    bannedEdge[x1][y1][x2][y2]=true;
    bannedEdge[x2][y2][x1][y1]=true;
    command.push({Time,1,x1,y1,x2,y2});  //record what operation we do at Time(Time is a variable)
    return true;  //banned successfully
}

bool LinkEdge(int x1,int y1,int x2,int y2){
    if(CheckLinkedEdge(x1,y1,x2,y2)) return false;
    linkedEdge[x1][y1][x2][y2]=true;
    linkedEdge[x2][y2][x1][y1]=true;
    command.push({Time,2,x1,y1,x2,y2});
    return true;
}

int CntlinkedEdge(int i,int j){
    return
    CheckLinkedEdge(TopLine)
    +CheckLinkedEdge(DownLine)
    +CheckLinkedEdge(LeftLine)
    +CheckLinkedEdge(RightLine);
}

int CntBannedEdge(int i,int j){
    return
    CheckBannedEdge(TopLine)
    +CheckBannedEdge(DownLine)
    +CheckBannedEdge(LeftLine)
    +CheckBannedEdge(RightLine);
}

//mark = ban + link
bool mark_edge_around_zero(){
   
    bool change=false; 
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            if(Map[i][j]==0){
                //-------ban-------
                // * . b .
                // * b 0 b
                // * . b .
                change|=BanEdge(TopLine);
                change|=BanEdge(DownLine);
                change|=BanEdge(LeftLine);
                change|=BanEdge(RightLine);
            }
        }
    }
    return change; //return whether it has done any operation this time
}

bool mark_edge_around_one(){
    bool change=false;
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            if(Map[i][j]==1){
                //-------basic------
                // * . b .
                // * b 1 |
                // * . b .
                if(CheckLinkedEdge(TopLine) || (CheckBannedEdge(DownLine) && CheckBannedEdge(RightLine) && CheckBannedEdge(LeftLine))){
                    change|=BanEdge(DownLine);
                    change|=BanEdge(RightLine);
                    change|=BanEdge(LeftLine);
                    change|=LinkEdge(TopLine);
                    continue;
                }else if(CheckLinkedEdge(DownLine) || CheckBannedEdge(TopLine) && CheckBannedEdge(RightLine) && CheckBannedEdge(LeftLine)){
                    change|=BanEdge(TopLine);
                    change|=BanEdge(RightLine);
                    change|=BanEdge(LeftLine);
                    change|=LinkEdge(DownLine);
                    continue;
                }else if(CheckLinkedEdge(LeftLine) || (CheckBannedEdge(TopLine) && CheckBannedEdge(RightLine) && CheckBannedEdge(DownLine))){
                    change|=BanEdge(TopLine);
                    change|=BanEdge(DownLine);
                    change|=BanEdge(RightLine);
                    change|=LinkEdge(LeftLine);
                    continue;
                }else if(CheckLinkedEdge(RightLine) || (CheckBannedEdge(TopLine) && CheckBannedEdge(LeftLine) && CheckBannedEdge(DownLine))){
                    change|=BanEdge(TopLine);
                    change|=BanEdge(DownLine);
                    change|=BanEdge(LeftLine);
                    change|=LinkEdge(RightLine);
                    continue;
                }
                //------ban------
                // *   x
                // * x . b .
                // *   b 1
                // *   .   .
                if(!goUp(i,j) && !goLeft(i,j)){
                    change|=BanEdge(TopLine);
                    change|=BanEdge(LeftLine);
                }
                if(!goUp(i,j+1) && !goRight(i,j+1)){
                    change|=BanEdge(TopLine);
                    change|=BanEdge(RightLine);
                }
                if(!goDown(i+1,j+1) && !goRight(i+1,j+1)){
                    change|=BanEdge(RightLine);
                    change|=BanEdge(DownLine);
                }
                if(!goDown(i+1,j) && !goLeft(i+1,j)){
                    change|=BanEdge(LeftLine);
                    change|=BanEdge(DownLine);
                }
            }
        }
    }
    return change;
}

bool mark_edge_around_two(){
    bool change=false;
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            if(Map[i][j]==2){
                //--------basic---------
                // . b .
                // | 2 |
                // . b .
                if((CheckLinkedEdge(TopLine) && CheckLinkedEdge(RightLine)) || (CheckBannedEdge(LeftLine) && CheckBannedEdge(DownLine))){
                    change|=BanEdge(DownLine);
                    change|=BanEdge(LeftLine);
                    change|=LinkEdge(TopLine);
                    change|=LinkEdge(RightLine);
                    continue;
                }else if((CheckLinkedEdge(TopLine) && CheckLinkedEdge(LeftLine)) || (CheckBannedEdge(DownLine) && CheckBannedEdge(RightLine))){
                    change|=BanEdge(DownLine);
                    change|=BanEdge(RightLine);
                    change|=LinkEdge(TopLine);
                    change|=LinkEdge(LeftLine);
                    continue;
                }else if((CheckLinkedEdge(TopLine) && CheckLinkedEdge(DownLine)) || (CheckBannedEdge(RightLine) && CheckBannedEdge(LeftLine))){
                    change|=BanEdge(LeftLine);
                    change|=BanEdge(RightLine);
                    change|=LinkEdge(TopLine);
                    change|=LinkEdge(DownLine);
                    continue;
                }else if((CheckLinkedEdge(DownLine) && CheckLinkedEdge(LeftLine)) || (CheckBannedEdge(TopLine) && CheckBannedEdge(RightLine))){
                    change|=BanEdge(RightLine);
                    change|=BanEdge(TopLine);
                    change|=LinkEdge(DownLine);
                    change|=LinkEdge(LeftLine);
                    continue;
                }else if((CheckLinkedEdge(DownLine) && CheckLinkedEdge(RightLine)) || (CheckBannedEdge(TopLine) && CheckBannedEdge(LeftLine))){
                    change|=BanEdge(LeftLine);
                    change|=BanEdge(TopLine);
                    change|=LinkEdge(RightLine);
                    change|=LinkEdge(DownLine);
                    continue;
                }else if((CheckLinkedEdge(LeftLine) && CheckLinkedEdge(RightLine)) || (CheckBannedEdge(TopLine) && CheckBannedEdge(DownLine))){
                    change|=BanEdge(DownLine);
                    change|=BanEdge(TopLine);
                    change|=LinkEdge(LeftLine);
                    change|=LinkEdge(RightLine);
                    continue;
                }
            }
        }
    }
    return change;
}

bool mark_edge_around_three(){
    bool change=false;
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            if(Map[i][j]==3){
                //------basic-------
                // . - .
                // | 3 |
                // . b .
                if(CheckBannedEdge(LeftLine) || (CheckLinkedEdge(TopLine) && CheckLinkedEdge(RightLine) && CheckLinkedEdge(DownLine))){
                    change|=BanEdge(LeftLine);
                    change|=LinkEdge(RightLine);
                    change|=LinkEdge(DownLine);
                    change|=LinkEdge(TopLine);
                    continue;
                }else if(CheckBannedEdge(DownLine) || (CheckLinkedEdge(TopLine) && CheckLinkedEdge(RightLine) && CheckLinkedEdge(LeftLine))){
                    change|=BanEdge(DownLine);
                    change|=LinkEdge(TopLine);
                    change|=LinkEdge(RightLine);
                    change|=LinkEdge(LeftLine);
                    continue;
                }else if(CheckBannedEdge(RightLine) || (CheckLinkedEdge(TopLine) && CheckLinkedEdge(LeftLine) && CheckLinkedEdge(DownLine))){
                    change|=BanEdge(RightLine);
                    change|=LinkEdge(LeftLine);
                    change|=LinkEdge(TopLine);
                    change|=LinkEdge(DownLine);
                    continue;
                }else if(CheckBannedEdge(TopLine) || (CheckLinkedEdge(DownLine) && CheckLinkedEdge(RightLine) && CheckLinkedEdge(LeftLine))){
                    change|=BanEdge(TopLine);
                    change|=LinkEdge(LeftLine);
                    change|=LinkEdge(RightLine);
                    change|=LinkEdge(DownLine);
                    continue;
                }
                //-------ban--------
                //   b   
                // - .   .
                //     3 
                //   .   .
                if(mp(i,j)==curPos){
                    if(mp(i,j-1)==prePos) change|=BanEdge(LeftTopLine);
                    if(mp(i-1,j)==prePos) change|=BanEdge(TopLeftLine);
                }else if(mp(i,j+1)==curPos){
                    if(mp(i-1,j+1)==prePos) change|=BanEdge(TopRightLine);
                    if(mp(i,j+2)==prePos) change|=BanEdge(RightTopLine);
                }else if(mp(i+1,j+1)==curPos){
                    if(mp(i+1,j+2)==prePos) change|=BanEdge(RightDownLine);
                    if(mp(i+2,j+1)==prePos) change|=BanEdge(DownRightLine);
                }else if(mp(i+1,j)==curPos){
                    if(mp(i+1,j-1)==prePos) change|=BanEdge(LeftDownLine);
                    if(mp(i+2,j)==prePos) change|=BanEdge(DownLeftLine);
                }
                //-------link-------
                // . -> .
                //   3  l
                // . l  .
                if(prePos!=StartPoint){
                    if(prePos.F==i && curPos.F==i && min(prePos.S,curPos.S)==j && !CheckLinkedEdge(prePos.S<curPos.S?LeftLine:RightLine)){
                        if(prePos.S<curPos.S && !CheckLinkedEdge(LeftLine)){
                            change|=LinkEdge(DownLine);
                            change|=LinkEdge(RightLine);
                        }
                        if(prePos.S>curPos.S && !CheckLinkedEdge(RightLine)){
                            change|=LinkEdge(DownLine);
                            change|=LinkEdge(LeftLine);
                        }
                    }else if(prePos.S==j+1 && curPos.S==j+1 && min(prePos.F,curPos.F)==i && !CheckLinkedEdge(prePos.F<curPos.F?TopLine:DownLine)){
                        if(prePos.F<curPos.F && !CheckLinkedEdge(TopLine)){
                            change|=LinkEdge(LeftLine);
                            change|=LinkEdge(DownLine);
                        }
                        if(prePos.F<curPos.F && !CheckLinkedEdge(DownLine)){
                            change|=LinkEdge(LeftLine);
                            change|=LinkEdge(TopLine);
                        }

                    }else if(prePos.S==j && curPos.S==j && min(prePos.F,curPos.F)==i && !CheckLinkedEdge(prePos.F<curPos.F?TopLine:DownLine)){
                        if(prePos.F<curPos.F && !CheckLinkedEdge(TopLine)){
                            change|=LinkEdge(RightLine);
                            change|=LinkEdge(DownLine);
                        }
                        if(prePos.F<curPos.F && !CheckLinkedEdge(DownLine)){
                            change|=LinkEdge(RightLine);
                            change|=LinkEdge(TopLine);
                        }

                    }else if(prePos.F==i+1 && curPos.F==i+1 && min(prePos.S,curPos.S)==j && !CheckLinkedEdge(prePos.S<curPos.S?LeftLine:RightLine)){
                        if(prePos.S<curPos.S && !CheckLinkedEdge(LeftLine)){
                            change|=LinkEdge(TopLine);
                            change|=LinkEdge(RightLine);
                        }
                        if(prePos.S<curPos.S && !CheckLinkedEdge(RightLine)){
                            change|=LinkEdge(TopLine);
                            change|=LinkEdge(LeftLine);
                        }

                    }
                }
                //------both---------
                //   . l .
                //     3
                // b . l . b
                //     3
                //   . l .
                if(Map[i+1][j]==3){
                    change|=LinkEdge(TopLine);
                    change|=LinkEdge(DownLine);
                    change|=LinkEdge(DownDownLine);
                    change|=BanEdge(DownLeftLine);
                    change|=BanEdge(DownRightLine);
                }
                //     b
                // .   .   .
                // l 3 l 3 l
                // .   .   .
                //     b
               if(Map[i][j+1]==3){
                    change|=LinkEdge(LeftLine);
                    change|=LinkEdge(RightLine);
                    change|=LinkEdge(RightRightLine);
                    change|=BanEdge(RightTopLine);
                    change|=BanEdge(RightDownLine);
                }
                //     b
                // . b . l . 
                // .   l 3
                // .   .   .
                if((!goLeft(i,j) && !goUp(i,j)) || (CheckLinkedEdge(TopLine) && CheckLinkedEdge(LeftLine))){
                    change|=BanEdge(TopLeftLine);
                    change|=BanEdge(LeftTopLine);
                    change|=LinkEdge(TopLine);
                    change|=LinkEdge(LeftLine);
                }
                if((!goUp(i,j+1) && !goRight(i,j+1)) || (CheckLinkedEdge(TopLine) && CheckLinkedEdge(RightLine))){
                    change|=BanEdge(RightTopLine);
                    change|=BanEdge(TopRightLine);
                    change|=LinkEdge(TopLine);
                    change|=LinkEdge(RightLine);
                }
                if((!goLeft(i+1,j) && !goDown(i+1,j)) || (CheckLinkedEdge(DownLine) && CheckLinkedEdge(LeftLine))){
                    change|=BanEdge(LeftDownLine);
                    change|=BanEdge(DownLeftLine);
                    change|=LinkEdge(DownLine);
                    change|=LinkEdge(LeftLine);
                }
                if((!goRight(i+1,j+1) && !goDown(i+1,j+1)) || (CheckLinkedEdge(DownLine) && CheckLinkedEdge(RightLine))){
                    change|=BanEdge(RightDownLine);
                    change|=BanEdge(DownRightLine);
                    change|=LinkEdge(DownLine);
                    change|=LinkEdge(RightLine);
                }
            }
        }
    }
    return change;
}

bool mark_edge_around_point(){
    bool change=false;
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            //   x  
            // x . x
            //   b 
            if(!goUp(i,j) && !goLeft(i,j) && !goRight(i,j)) change|=BanEdge(LeftLine);
            if(!goUp(i,j) && !goLeft(i,j) && !goDown(i,j)) change|=BanEdge(TopLine);
            if(!goDown(i,j) && !goLeft(i,j) && !goRight(i,j)) change|=BanEdge(LeftTopLine);
            if(!goUp(i,j) && !goDown(i,j) && !goRight(i,j)) change|=BanEdge(TopLeftLine);
        }
    }
    return change;
}

bool heuristic(){  //ban and link edge
    while(true){
        bool change=false;
        change|=mark_edge_around_zero();
        change|=mark_edge_around_one(); 
        change|=mark_edge_around_two();
        change|=mark_edge_around_three();
        change|=mark_edge_around_point();
        if(!change) break;
    }
    
    //check for legitimate
    for(int i=1;i<=n;i++){  
        for(int j=1;j<=m;j++){
            if(CheckBannedEdge(TopLine) && CheckLinkedEdge(TopLine)) return false;
            if(CheckBannedEdge(DownLine) && CheckLinkedEdge(DownLine)) return false;
            if(CheckBannedEdge(LeftLine) && CheckLinkedEdge(LeftLine)) return false;
            if(CheckBannedEdge(RightLine) && CheckLinkedEdge(RightLine)) return false;

            if(CheckLinkedEdge(TopLine)+
                CheckLinkedEdge(LeftLine)+
                CheckLinkedEdge(TopLeftLine)+
                CheckLinkedEdge(LeftTopLine) > 2) return false;
        }
    }
    return true;
}

void unheuristic(int Time){
    while(command.top().time>=Time){ //remove operations done after Time
        cmd item=command.top();command.pop();
        if(item.type==1){ //unban
            bannedEdge[item.x1][item.y1][item.x2][item.y2]=false;
            bannedEdge[item.x2][item.y2][item.x1][item.y1]=false;
        }else{  //unlink
            linkedEdge[item.x1][item.y1][item.x2][item.y2]=false;
            linkedEdge[item.x2][item.y2][item.x1][item.y1]=false;
        }
    }
}

bool check(){  //check the generated Map satisfy the requirements
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            if(Map[i][j]!=-1 && usedEdgeCnt[i][j]!=Map[i][j]) return false;
        }
    }
    return true;
}

void update_usedEdgeCnt(int x1,int y1,int x2,int y2,int val){  //calculate sum of used edges
    if(x1-x2!=0){  //vertical
        usedEdgeCnt[min(x1,x2)][y1]+=val;
        usedEdgeCnt[min(x1,x2)][y1-1]+=val;
    }else{  //horizontal
        usedEdgeCnt[x1][min(y1,y2)]+=val;
        usedEdgeCnt[x1-1][min(y1,y2)]+=val;
    }
}

bool isRoot=true;
int dfs(int x,int y){
    if(!isRoot && mp(x,y)==StartPoint) return check();   //get a circle
    isRoot=false;

    //go linked road first
    int LinkedRoadCnt=0;
    pii LinkedRoad;
    for(int i=0;i<4;i++){  //find linked road
        int nx=x+dx[i],ny=y+dy[i];
        if(mp(nx,ny)==prePos) continue;
        if(CheckLinkedEdge(x,y,nx,ny)){
            if(CheckBannedEdge(x,y,nx,ny) || usedNode[nx][ny]) return false;
            LinkedRoadCnt++;
            LinkedRoad=mp(nx,ny);
        }
    }
    
    if(LinkedRoadCnt > 2) return false;
    if(LinkedRoadCnt == 2 && mp(x,y)!=StartPoint){
        return false;
        //only startPoint can have two linked roads
        // . l .
        // b 3 l
        // . l . <- start from here
    }
    if(LinkedRoadCnt > 0){  //go linked road only, beacuse linked means "need" to go
        int nx=LinkedRoad.F,ny=LinkedRoad.S;
        
        prePos={x,y},curPos={nx,ny}; //set previous node and current node

        //add this edge
        usedNode[nx][ny]=true;
        update_usedEdgeCnt(x,y,nx,ny,1);
        int CurTime=++Time;
        bool legal=heuristic();

        if(legal && dfs(nx,ny)) return true;  //find legal Graph

        //remove this edge
        usedNode[nx][ny]=false;
        update_usedEdgeCnt(x,y,nx,ny,-1);
        unheuristic(CurTime);

    }else{  //no linked road, go normal road
        for(int i=0;i<4;i++){
            int nx=x+dx[i],ny=y+dy[i];
            if(usedNode[nx][ny] || CheckBannedEdge(x,y,nx,ny)) continue;

            prePos={x,y},curPos={nx,ny};
            
            usedNode[nx][ny]=true;
            linkedEdge[x][y][nx][ny]=true;
            update_usedEdgeCnt(x,y,nx,ny,1);
            int CurTime=++Time;
            bool legal=heuristic();
        
            if(legal && dfs(nx,ny)) return true;

            usedNode[nx][ny]=false;
            linkedEdge[x][y][nx][ny]=false;
            update_usedEdgeCnt(x,y,nx,ny,-1);
            unheuristic(CurTime);
        }
    }
    return false;
}

void ReadInput(){
    string s;
    while(cin >> s){
        m=s.size(),n++;
        for(int i=1;i<=m;i++){
            if(isdigit(s[i-1])) Map[n][i]=s[i-1]-'0';
            else Map[n][i]=-1;
        }
    }
}

void Initalization(){
    //ban edge out of range
    for(int i=1;i<=m+1;i++){
        BanEdge(1,i,0,i);
        BanEdge(n+1,i,n+2,i);
    }
    for(int i=1;i<=n+1;i++){
        BanEdge(i,1,i,0);
        BanEdge(i,m+1,i,m+2);
    }
    heuristic(); //ban or link edge in graph before finding solution
}

void FindSolution(){
    Initalization();
    
    //find start point : 3 > 2 > 1
    vector<pii> startPoint(4);
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            if(Map[i][j]==1) startPoint[1]=mp(i,j);
            else if(Map[i][j]==2) startPoint[2]=mp(i,j);
            else if(Map[i][j]==3) startPoint[3]=mp(i,j);

            if(Map[i][j]==3) break;  //4 node around 3 must in solution
        }
    }
    
    queue<pii> q;
    for(int i=3;i>=1;i--){  //start from 3
        int x=startPoint[i].F,y=startPoint[i].S;
        if(x && y){
            q.push(mp(x+1,y+1));
            q.push(mp(x+1,y));
            q.push(mp(x,y+1));
            q.push(mp(x,y));
            break;
        }
    }
    
    while(!q.empty()){
        StartPoint=q.front(); q.pop();
        isRoot=true;
        if(dfs(StartPoint.F,StartPoint.S)) return;  //found the solution
    }
}

void PrintGraph(){ //print graph
    for(int i=1;i<=m;i++){
        cout << "." << (CheckLinkedEdge(1,i,1,i+1)?"___":"   ");
    }
    cout << ".\n";
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            cout << (CheckLinkedEdge(i,j,i+1,j)?'|':' ') << ' ' << (Map[i][j]!=-1?char(Map[i][j]+'0'):' ') << ' ';
        }
        cout << (CheckLinkedEdge(i,m+1,i+1,m+1)?'|':' ') << '\n';

        for(int j=1;j<=m;j++){
            cout << (CheckLinkedEdge(i,j,i+1,j)?'!':'.') << (CheckLinkedEdge(i+1,j,i+1,j+1)?"___":"   ");
        }

        cout << (CheckLinkedEdge(i+1,m+1,i,m+1)?'!':'.') << '\n';
        
    }
}

int main(){
    cin.sync_with_stdio(0),cin.tie(0);
    
    ReadInput();
    FindSolution();
    PrintGraph();
}
