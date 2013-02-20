

bool InitRedux();
void ReleaseRedux();


int Uncompress_Mode1(uint8_t *src, uint32_t src_size, uint8_t *dest);
int Uncompress_Mode2(uint8_t *src, uint32_t src_size, uint8_t *dest, uint32_t& ptr_dest_size);
int Uncompress_Mode3(uint8_t *src, uint32_t src_size, uint8_t *dest, uint32_t* dest_size);
int Uncompress_Mode4(uint8_t *src, uint32_t src_size, uint8_t *dest, uint32_t dest_size);
