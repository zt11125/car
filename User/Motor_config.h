#define STANDARD_SPEED 50.0f
#define SPEED_RAGE 50.0f

typedef enum {
		ST_STAIGHT=1,				/*S1*/
    ST_CIRCLE,        /* S2 */
    ST_SQUARE,        /* S3*/
    ST_OVAL,          /* S4*/
    ST_STAR,          /* S5加分，先占位 */
    ST_FIXED,         /* S6 定距加分，先占位 */
    ST_FAULT          /* 故障 */
} CarState_t;

