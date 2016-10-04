
#pragma once

#include <string>

class PushClient;

class PushClientFactory {
public:
    static PushClientFactory * GetDefault();

public:
    PushClientFactory();
    ~PushClientFactory();

    PushClient & Get();
};

