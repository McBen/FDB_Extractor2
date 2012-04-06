

bool InitRedux();
void ReleaseRedux();


int Uncompress_Mode1(BYTE *src, unsigned int src_size, BYTE *dest);
int Uncompress_Mode2(BYTE *src, unsigned int src_size, BYTE *dest, unsigned int& ptr_dest_size);
int Uncompress_Mode3(BYTE *src, unsigned int src_size, BYTE *dest, unsigned int* dest_size);
int Uncompress_Mode4(BYTE *src, unsigned int src_size, BYTE *dest, unsigned int dest_size);
