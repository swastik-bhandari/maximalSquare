/////// in this code , datatype of elements of matrix is boolean //////////////////
#include <iostream>
#include <vector>
using namespace std;

int max_area(vector<vector<bool>>& v) {
  int row_size = v[0].size();
int col_size = v.size();
int found_max_width = 0;
int check_max_width =1;
int check_max_height=1;
int counter =0;
vector<int>freq(row_size, 0);

for(int i = 0 ; i<col_size ; i++) {

for(int j=0 ; j<row_size; j++) {
if(v[i][j]) {
freq[j]++;
if(check_max_height <= freq[j]) {
counter++;
if(check_max_width == counter) {
found_max_width++;
check_max_width++;
check_max_height++;
counter=0;
}
continue;
}
} ///////////////// 1
else {
freq[j]=0;
} /////////////// 0
counter=0;
}
counter = 0; /////// 
}
return found_max_width*found_max_width;
}

int main() {
    vector<vector<bool>> v = {
        {1,1,0,1},
        {1,1,0,1},
        {1,1,1,1},
    
        
    };

    cout << max_area(v);
    return 0;
}