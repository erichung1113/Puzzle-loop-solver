#include <bits/stdc++.h>
using namespace std;
#pragma GCC optimize("O2","unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#define pb push_back
#define pii pair<int,int>
#define F first
#define S second
#define mp make_pair
const int MAXN=10;

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
bool usedNode[MAXN][MAXN];
bool linkedEdge[MAXN][MAXN][MAXN][MAXN];  
bool bannedEdge[MAXN][MAXN][MAXN][MAXN];
int usedEdgeCnt[MAXN][MAXN];
//Edge[x1][x2][y1][y2] : (x1,y1)->(x2,y2)
int n,m,Time;
pii StartPoint,curPos,nxtPos;

typedef struct cmd{
    int time,type,x1,y1,x2,y2;
}cmd;
stack<cmd> command; // use to restore linked or banned edge

bool CheckBannedEdge(int x1,int y1,int x2,int y2){
    return bannedEdge[x1][y1][x2][y2] || bannedEdge[x2][y2][x1][y1];
}
bool CheckLinkedEdge(int x1,int y1,int x2,int y2){
    return linkedEdge[x1][y1][x2][y2] || linkedEdge[x2][y2][x1][y1];
}

bool goUp(int x,int y){
    
    if(mp(x-1,y)==nxtPos || CheckLinkedEdge(x,y,x-1,y)) return true;
    if(CheckBannedEdge(x,y,x-1,y) || usedNode[x-1][y]) return false;
    return true;
}
bool goDown(int x,int y){
    if(mp(x+1,y)==nxtPos || CheckLinkedEdge(x,y,x+1,y)) return true;
    if(CheckBannedEdge(x,y,x+1,y) || usedNode[x+1][y]) return false;
    return true;
}
bool goLeft(int x,int y){
    if(mp(x,y-1)==nxtPos || CheckLinkedEdge(x,y,x,y-1)) return true;
    if(CheckBannedEdge(x,y,x,y-1) || usedNode[x][y-1]) return false;
    return true;
}
bool goRight(int x,int y){
    if(mp(x,y+1)==nxtPos || CheckLinkedEdge(x,y,x,y+1)) return true;
    if(CheckBannedEdge(x,y,x,y+1) || usedNode[x][y+1]) return false;

    return true;
}

bool BanEdge(int x1,int y1,int x2,int y2){ 
    if(bannedEdge[x1][y1][x2][y2]) return false;  //banned already
    bannedEdge[x1][y1][x2][y2]=true;
    bannedEdge[x2][y2][x1][y1]=true;
    command.push({Time,1,x1,y1,x2,y2});  //record what operation we do at Time
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
    //-------ban-------
    /**
     * . b .
     * b 0 b
     * . b .
     */
    bool change=false; 
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            if(Map[i][j]==0){
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
                //-------ban---------
                if(CntlinkedEdge(i,j)==0){
                    /**
                     *   x
                     * x . b .
                     *   b 1
                     *   .   .
                    */
                    if(!goUp(i,j) && !goLeft(i,j) && mp(i,j)!=nxtPos && mp(i,j)!=StartPoint){
                        change|=BanEdge(TopLine);
                        change|=BanEdge(LeftLine);
                    }
                    if(!goUp(i,j+1) && !goRight(i,j+1) && mp(i,j+1)!=nxtPos && mp(i,j+1)!=StartPoint){
                        change|=BanEdge(TopLine);
                        change|=BanEdge(RightLine);
                    }
                    if(!goDown(i+1,j+1) && !goRight(i+1,j+1) && mp(i+1,j+1)!=nxtPos && mp(i+1,j+1)!=StartPoint){
                        change|=BanEdge(RightLine);
                        change|=BanEdge(DownLine);
                    }
                    if(!goDown(i+1,j) && !goLeft(i+1,j) && mp(i+1,j)!=nxtPos && mp(i+1,j)!=StartPoint){
                        change|=BanEdge(LeftLine);
                        change|=BanEdge(DownLine);
                    }
                }
                //-------both------
                /**
                 * . b .
                 * b 1 |
                 * . b .
                */ 
                if(CheckLinkedEdge(TopLine) || (CheckBannedEdge(DownLine) && CheckBannedEdge(RightLine) && CheckBannedEdge(LeftLine))){
                    change|=BanEdge(DownLine);
                    change|=BanEdge(RightLine);
                    change|=BanEdge(LeftLine);
                    change|=LinkEdge(TopLine);

                }else if(CheckLinkedEdge(DownLine) || CheckBannedEdge(TopLine) && CheckBannedEdge(RightLine) && CheckBannedEdge(LeftLine)){
                    change|=BanEdge(TopLine);
                    change|=BanEdge(RightLine);
                    change|=BanEdge(LeftLine);
                    change|=LinkEdge(DownLine);

                }else if(CheckLinkedEdge(LeftLine) || (CheckBannedEdge(TopLine) && CheckBannedEdge(RightLine) && CheckBannedEdge(DownLine))){
                    change|=BanEdge(TopLine);
                    change|=BanEdge(DownLine);
                    change|=BanEdge(RightLine);
                    change|=LinkEdge(LeftLine);

                }else if(CheckLinkedEdge(RightLine) || (CheckBannedEdge(TopLine) && CheckBannedEdge(LeftLine) && CheckBannedEdge(DownLine))){
                    change|=BanEdge(TopLine);
                    change|=BanEdge(DownLine);
                    change|=BanEdge(LeftLine);
                    change|=LinkEdge(RightLine);
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
                /**
                 * . b .
                 * | 2 |
                 * . b .
                 */
                if((CheckLinkedEdge(TopLine) && CheckLinkedEdge(RightLine)) || (CheckBannedEdge(LeftLine) && CheckBannedEdge(DownLine))){
                    change|=BanEdge(DownLine);
                    change|=BanEdge(LeftLine);
                    change|=LinkEdge(TopLine);
                    change|=LinkEdge(RightLine);

                }else if((CheckLinkedEdge(TopLine) && CheckLinkedEdge(LeftLine)) || (CheckBannedEdge(DownLine) && CheckBannedEdge(RightLine))){
                    change|=BanEdge(DownLine);
                    change|=BanEdge(RightLine);
                    change|=LinkEdge(TopLine);
                    change|=LinkEdge(LeftLine);

                }else if((CheckLinkedEdge(TopLine) && CheckLinkedEdge(DownLine)) || (CheckBannedEdge(RightLine) && CheckBannedEdge(LeftLine))){
                    change|=BanEdge(LeftLine);
                    change|=BanEdge(RightLine);
                    change|=LinkEdge(TopLine);
                    change|=LinkEdge(DownLine);

                }else if((CheckLinkedEdge(DownLine) && CheckLinkedEdge(LeftLine)) || (CheckBannedEdge(TopLine) && CheckBannedEdge(RightLine))){
                    change|=BanEdge(RightLine);
                    change|=BanEdge(TopLine);
                    change|=LinkEdge(DownLine);
                    change|=LinkEdge(LeftLine);

                }else if((CheckLinkedEdge(DownLine) && CheckLinkedEdge(RightLine)) || (CheckBannedEdge(TopLine) && CheckBannedEdge(LeftLine))){
                    change|=BanEdge(LeftLine);
                    change|=BanEdge(TopLine);
                    change|=LinkEdge(RightLine);
                    change|=LinkEdge(DownLine);

                }else if((CheckLinkedEdge(LeftLine) && CheckLinkedEdge(RightLine)) || (CheckBannedEdge(TopLine) && CheckBannedEdge(DownLine))){
                    change|=BanEdge(DownLine);
                    change|=BanEdge(TopLine);
                    change|=LinkEdge(LeftLine);
                    change|=LinkEdge(RightLine);
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
                /**
                 * . - .
                 * | 3 |
                 * . b .
                 */
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
                /**
                 *   b   
                 * - .   .
                 *     3 
                 *   .   .
                 */
                if(mp(i,j)==nxtPos){
                    if(mp(i,j-1)==curPos) change|=BanEdge(LeftTopLine);
                    if(mp(i-1,j)==curPos) change|=BanEdge(TopLeftLine);
                }else if(mp(i,j+1)==nxtPos){
                    if(mp(i-1,j+1)==curPos) change|=BanEdge(TopRightLine);
                    if(mp(i,j+2)==curPos) change|=BanEdge(RightTopLine);
                }else if(mp(i+1,j+1)==nxtPos){
                    if(mp(i+1,j+2)==curPos) change|=BanEdge(RightDownLine);
                    if(mp(i+2,j+1)==curPos) change|=BanEdge(DownRightLine);
                }else if(mp(i+1,j)==nxtPos){
                    if(mp(i+1,j-1)==curPos) change|=BanEdge(LeftDownLine);
                    if(mp(i+2,j)==curPos) change|=BanEdge(DownLeftLine);
                }
                //-------link-------
                /**
                 * . -> .
                 *   3  l
                 * . l  .
                 */
                if(curPos!=StartPoint && CntlinkedEdge(i,j)==1){;
                    if(CheckLinkedEdge(TopLine)){
                        if(curPos.S<nxtPos.S && CheckLinkedEdge(LeftLine)) continue;
                        if(curPos.S>nxtPos.S && CheckLinkedEdge(RightLine)) continue;

                        change|=LinkEdge(DownLine);
                        
                        if(curPos.S<nxtPos.S) change|=LinkEdge(RightLine);
                        else change|=LinkEdge(LeftLine);

                    }else if(CheckLinkedEdge(RightLine) ){
                        if(curPos.F<nxtPos.F && CheckLinkedEdge(TopLine)) continue;
                        if(curPos.F>nxtPos.F && CheckLinkedEdge(DownLine)) continue;

                        change|=LinkEdge(LeftLine);
                        
                        if(curPos.F<nxtPos.F) change|=LinkEdge(DownLine);
                        else change|=LinkEdge(TopLine);

                    }else if(CheckLinkedEdge(LeftLine)){
                        if(curPos.F<nxtPos.F && CheckLinkedEdge(TopLine)) continue;
                        if(curPos.F>nxtPos.F && CheckLinkedEdge(DownLine)) continue;

                        change|=LinkEdge(RightLine);
                        
                        if(curPos.F<nxtPos.F) change|=LinkEdge(DownLine);
                        else change|=LinkEdge(TopLine);

                    }else if(CheckLinkedEdge(DownLine)){
                        if(curPos.S<nxtPos.S && CheckLinkedEdge(LeftLine)) continue;
                        if(curPos.S>nxtPos.S && CheckLinkedEdge(RightLine)) continue;

                        change|=LinkEdge(TopLine);
                        
                        if(curPos.S<nxtPos.S) change|=LinkEdge(RightLine);
                        else change|=LinkEdge(LeftLine);

                    }
                }
                
                //------both---------
                /**
                *   . l .
                *     3
                * b . l . b
                *     3
                *   . l .
                */ 
                if(Map[i+1][j]==3){
                    change|=LinkEdge(TopLine);
                    change|=LinkEdge(DownLine);
                    change|=LinkEdge(DownDownLine);
                    change|=BanEdge(DownLeftLine);
                    change|=BanEdge(DownRightLine);
                }
                /**
                *     b
                * .   .   .
                * l 3 l 3 l
                * .   .   .
                *     b
                */
               if(Map[i][j+1]==3){
                    change|=LinkEdge(LeftLine);
                    change|=LinkEdge(RightLine);
                    change|=LinkEdge(RightRightLine);
                    change|=BanEdge(RightTopLine);
                    change|=BanEdge(RightDownLine);
                }
                /**
                 *       b
                 *   . b . l . 
                 *   .   l 3
                 *   .   .   .
                 */
                
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
            if(!goUp(i,j) && !goLeft(i,j) && !goRight(i,j)) change|=BanEdge(LeftLine);
            if(!goUp(i,j) && !goLeft(i,j) && !goDown(i,j)) change|=BanEdge(TopLine);
            if(!goDown(i,j) && !goLeft(i,j) && !goRight(i,j)) change|=BanEdge(LeftTopLine);
            if(!goUp(i,j) && !goDown(i,j) && !goRight(i,j)) change|=BanEdge(TopLeftLine);
        }
    }
    return change;
}

int T=0;
void PrintGraph();
void PrintBan();
void PrintLink();

int dfsCnt=0;
void debug(int l,int r){
    dfsCnt++;
    if(l<=dfsCnt && dfsCnt<=r){
        cout << nxtPos.F << ' ' << nxtPos.S << '\n';
        PrintGraph();
        PrintBan();
        // PrintLink();
        
        cout << "\n\n\n";   
    }
}

void heuristic(){  //ban and link edge
    while(true){
        bool change=false;
        // debug(0,10000);
        change|=mark_edge_around_zero();
        change|=mark_edge_around_one(); 
        change|=mark_edge_around_two();
        change|=mark_edge_around_three();
        // change|=mark_edge_around_point();
        if(!change) break;
        
    }
}

void unheuristic(int Time){
    while(command.top().time>=Time){ //remove operations which is done after Time
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

bool check(){  //check the generated Map is satisfy the requires or not
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            if(Map[i][j]==-1) continue;
            if(usedEdgeCnt[i][j]!=Map[i][j]) return false;
        }
    }
    return true;
}


void update_usedEdgeCnt(int x1,int y1,int x2,int y2,int val){
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
    if(!isRoot && mp(x,y)==StartPoint) return check();
    isRoot=false;

    // debug(0,10000);

    //go linked road first
    int LinkedRoadCnt=0;
    pii LinkedRoad;
    for(int i=0;i<4;i++){  //cnt and record linked road
        int nx=x+dx[i],ny=y+dy[i];
        if(mp(nx,ny)==curPos) continue;

        if(CheckLinkedEdge(x,y,nx,ny)){
            if(CheckBannedEdge(x,y,nx,ny) || usedNode[nx][ny]) return false;
            LinkedRoadCnt++;
            LinkedRoad=mp(nx,ny);
        }
    }
    if(LinkedRoadCnt > 2) return false;
    if(LinkedRoadCnt == 2 && mp(x,y)!=StartPoint){
        return false;
        /**  only startPoint can have two linked roads
         * . l .
         * b 3 l
         * . l . <- startPoint at here
         */
    }
    if(LinkedRoadCnt > 0){  //go linked road only, beacuse linked means "need" to go
        int nx=LinkedRoad.F,ny=LinkedRoad.S;
        
        curPos={x,y},nxtPos={nx,ny}; //set current node and next node to go

        //add this edge and do ban or link operation
        
        usedNode[nx][ny]=true;
        update_usedEdgeCnt(x,y,nx,ny,1);
        if(dfs(nx,ny)) return true;  //find legal Graph
        update_usedEdgeCnt(x,y,nx,ny,-1);
        //remove this edge and undo ban or link operation
        
        usedNode[nx][ny]=false;
        

    }else{  //no linked road, go normal road
        for(int i=0;i<4;i++){
            int nx=x+dx[i],ny=y+dy[i];
            if(usedNode[nx][ny] || CheckBannedEdge(x,y,nx,ny)) continue;

            curPos={x,y},nxtPos={nx,ny};

            linkedEdge[x][y][nx][ny]=true;
            
            usedNode[nx][ny]=true;
            int CurTime=++Time;
            heuristic();
            update_usedEdgeCnt(x,y,nx,ny,1);
            if(dfs(nx,ny)) return true;
            update_usedEdgeCnt(x,y,nx,ny,-1);
            linkedEdge[x][y][nx][ny]=false;
            
            usedNode[nx][ny]=false;
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
            if(s[i-1]=='.') Map[n][i]=-1;
            else Map[n][i]=s[i-1]-'0';
        }
    }
}

void Initalization(){
    //ban edge which is out of range
    for(int i=1;i<=m+1;i++){
        BanEdge(1,i,0,i);
        BanEdge(n+1,i,n+2,i);
    }
    for(int i=1;i<=n+1;i++){
        BanEdge(i,1,i,0);
        BanEdge(i,m+1,i,m+2);
    }
    heuristic(); //ban or link edge in graph
}

void FindSolution(){
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
    // cout << "Graph:\n";
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

void PrintBan(){ 
    cout << "Ban:\n";
    for(int i=1;i<=m;i++){
        cout << "." << (CheckBannedEdge(1,i,1,i+1)?"___":"   ");
    }
    cout << ".\n";
    for(int i=1;i<=n;i++){
        for(int j=1;j<=m;j++){
            cout << (CheckBannedEdge(i,j,i+1,j)?'|':' ') << ' ' << (Map[i][j]!=-1?char(Map[i][j]+'0'):' ') << ' ';
        }
        cout << (CheckBannedEdge(i,m+1,i+1,m+1)?'|':' ') << '\n';

        for(int j=1;j<=m;j++){
            cout << (CheckBannedEdge(i,j,i+1,j)?'!':'.') << (CheckBannedEdge(i+1,j,i+1,j+1)?"___":"   ");
        }

        cout << (CheckBannedEdge(i+1,m+1,i,m+1)?'!':'.') << '\n';
        
    }
}

void PrintLink(){ 
    cout << "Link:\n";
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
    Initalization();
    FindSolution();
    PrintGraph();
}
