#include <core/result.h>

BEGIN_ENUM_IMPL(Type) {
    {"Done",(uint32_t)EType::Done},
{"Wait",(uint32_t)EType::Wait},
{"Error",(uint32_t)EType::Error},
{"Finish",(uint32_t)EType::Finish},
{"Async",(uint32_t)EType::Async},
};
END_ENUM_IMPL(Type);