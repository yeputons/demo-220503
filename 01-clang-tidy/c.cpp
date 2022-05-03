int get_local_id() { return 0; }
int main() {
    int x = get_local_id();
    while (x) {}
}
