//
// Created by Stanislau Senkevich on 21.3.24.
//

#ifndef LAB_5_PROJECT_CONFIG_H
#define LAB_5_PROJECT_CONFIG_H

#include <string>

class Config {
private:
    double driverPercent = 20.;

public:
    [[nodiscard]] double getDriverMul() const {
        return driverPercent / 100.;
    }

    void setDriverPercent(double newDriverPercent) {
        driverPercent = newDriverPercent;
    }
};



#endif //LAB_5_PROJECT_CONFIG_H
