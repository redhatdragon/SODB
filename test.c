#include "SODB.h"

struct TestStruct {
    float x, y, z;
};

int main(){
    SODBInit("test.dat", sizeof(struct TestStruct));
    struct TestStruct ts = {1.2, 2.3, 3.4};
    printf("%f", ts.x); printf(" ");
    printf("%f", ts.y); printf(" ");
    printf("%f", ts.z); printf(" ");
    printf("\n");
    printf("\n");

    SODBAppend("test.dat", &ts);
    ts.x = 5;
    SODBAppend("test.dat", &ts);
    ts.y = ts.z = 50;
    SODBAppend("test.dat", &ts);

    struct TestStruct nts;
    SODBGet("test.dat", 0, &nts);
    printf("%f", nts.x); printf(" ");
    printf("%f", nts.y); printf(" ");
    printf("%f", nts.z); printf(" ");
    printf("\n");
    SODBGet("test.dat", 1, &nts);
    printf("%f", nts.x); printf(" ");
    printf("%f", nts.y); printf(" ");
    printf("%f", nts.z); printf(" ");
    printf("\n");
    SODBGet("test.dat", 2, &nts);
    printf("%f", nts.x); printf(" ");
    printf("%f", nts.y); printf(" ");
    printf("%f", nts.z); printf(" ");
    printf("\n");

    system("pause");
    return 0;
}