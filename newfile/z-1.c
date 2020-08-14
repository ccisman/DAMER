int x, y, z;
int f(int s) {
z=s+1;
return z;
}
int main() {
int q;
x=1;
y=f(x); 
f(1);
q=y;//q=y+1
}
