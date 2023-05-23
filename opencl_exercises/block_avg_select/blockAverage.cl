__kernel void blockAverageGElements ( __global ushort *input,
    __global ushort *output,
    int image_width,
    int image_height,
    int block_width,
    int block_height
)
{
    int col_id = get_global_id (0);
    int row_id = get_global_id (1);
    int width = get_global_size(0);
    int bl_off, bl_1st_off, bl_2nd_off;
    int x,y;
    uint accumulated;
    ushort4 in1,in2,out;
    int4 mask = {0, -1, 0, -1};

    //(row_id * block_height) * image_width + (col_id * block_width);
    bl_off = mad24(row_id*block_height, image_width, col_id * block_width);
    for (y=0; y<block_height; y+=2) {
        bl_1st_off = mad24(y+0, image_width, bl_off);
        bl_2nd_off = mad24(y+1, image_width, bl_off);
        
        for(x=0;x<block_width;x+=4) {
            in1 = vload4(0, input + bl_1st_off + x);
            in2 = vload4(0, input + bl_2nd_off + x);
            out = select(in1, in2, mask);
            accumulated += out.x + out.y + out.z + out.w;
        }
    }

    output[row_id * width + col_id] = (2*accumulated) / (block_width*block_height);
}

