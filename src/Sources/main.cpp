#include "api.h"

int main() {
    Api::Init();
    Api::LoadScene("demo");
    Api::Run();

    return 0;
}
