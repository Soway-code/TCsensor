#include "algorithm.h"

//******************************************************************************
// 名称         : SortArrayExtreme()
// 创建日期     : 2016-09-05
// 作者         : 庄明群
// 功能         : 把数组内的元素按从小到大的顺序排序
// 输入参数     : u16 Array[]               数组
//                const u32 ArraySize       排序的长度
//                const u32 SortHeadSize    数组头部元素个数   
//                const u32 SortTailSize    数组尾部元素个数
// 输出参数     : 无
// 返回结果     : 无
// 注意和说明   : 
// 修改内容     :
//******************************************************************************
static void SortArrayExtreme(int Array[], const uint32_t ArraySize,
                      const uint32_t SortHeadSize, const uint32_t SortTailSize)
{
    uint32_t i, j;
    int temp;

    for (i = 0; i < SortTailSize; i++)
    {
        for (j = 0; j < ArraySize - i - 1; j++)
        {
            if (Array[j] > Array[j+1])
            {
                temp = Array[j];
                Array[j] = Array[j+1];
                Array[j+1] = temp;
            }
        }
    }

    for (i = 0; i < SortHeadSize; i++)
    {
        for (j = ArraySize - SortTailSize - 1 ; j > i; j--)
        {
            if (Array[j - 1] > Array[j])
            {
                temp = Array[j - 1];
                Array[j - 1] = Array[j];
                Array[j] = temp;
            }
        }
    }
}



//******************************************************************************
// 名称         : GetAverage()
// 创建日期     : 2016-09-05
// 作者         : 庄明群
// 功能         : 去掉最大和最小求均值
// 输入参数     : u16 Array[]               数组
//                const u32 ArraySize       排序的长度
//                const u32 SortHeadSize    数组头部元素个数   
//                const u32 SortTailSize    数组尾部元素个数
// 输出参数     : 无
// 返回结果     : 平均值
// 注意和说明   : 
// 修改内容     :
//******************************************************************************
static int GetAverage(int Array[], const uint32_t ArraySize,
               const uint32_t DelHeadSize, const uint32_t DelTailSize)
{
    long long int sum = 0;

    if ((DelHeadSize + DelTailSize) >= ArraySize)
    {
        return 0;
    }

    for (long i = DelHeadSize; i < ArraySize - DelTailSize; i++)
    {
        sum += Array[i];
    }

    return(sum / (ArraySize - DelHeadSize - DelTailSize));
}



//******************************************************************************
// 名称         : GetDelExtremeAndAverage()
// 创建日期     : 2016-09-05
// 作者         : 庄明群
// 功能         : 排序并求均值
// 输入参数     : u16 Array[]               数组
//                const u32 ArraySize       排序的长度
//                const u32 SortHeadSize    数组头部元素个数   
//                const u32 SortTailSize    数组尾部元素个数
// 输出参数     : 无
// 返回结果     : 平均值
// 注意和说明   : 
// 修改内容     :
//******************************************************************************
int GetDelExtremeAndAverage(int Array[], const uint32_t ArraySize,
                            const uint32_t SortHeadSize, const uint32_t SortTailSize)
{
    SortArrayExtreme(Array, ArraySize, SortHeadSize, SortTailSize);
    return(GetAverage(Array, ArraySize, SortHeadSize, SortTailSize));
}

