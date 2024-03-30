//
// Created by Stanislau Senkevich on 21.3.24.
//

#include "../include/validator.h"

const std::regex Validator::name_pattern(R"(^[A-Za-z]+$)");
const std::regex Validator::address_pattern(R"(^[a-zA-Z0-9\\s.,/]+$)");
const std::regex Validator::city_pattern(R"(^[A-Za-z]+$)");
const std::regex Validator::date_pattern(R"(^\d{4}-\d{2}-\d{2}$)");
const std::regex Validator::license_pattern("^[0-9]{4}[ABEIKMHOPCTX]{2}-[1-7]$");
const std::regex Validator::login_pattern("^[A-Za-z0-9_]{1,}$");

bool Validator::validLicense(const std::string &license) {
    return std::regex_match(license, license_pattern);
}

bool Validator::validPeriod(const std::string &date_start, const std::string &date_end) {
    if (!std::regex_match(date_start, date_pattern) || !std::regex_match(date_end, date_pattern)) {
        throw std::invalid_argument("Date must be provided in format YYYY-MM-DD");
    }

    struct std::tm start_tm = {}, end_tm = {};
    std::istringstream start_ss(date_start), end_ss(date_end);
    start_ss >> start_tm.tm_year;
    start_ss.ignore();
    start_ss >> start_tm.tm_mon;
    start_ss.ignore();
    start_ss >> start_tm.tm_mday;

    end_ss >> end_tm.tm_year;
    end_ss.ignore();
    end_ss >> end_tm.tm_mon;
    end_ss.ignore();
    end_ss >> end_tm.tm_mday;

    std::time_t start_time = std::mktime(&start_tm);
    std::time_t end_time = std::mktime(&end_tm);

    return start_time <= end_time;
}

bool Validator::validAge(const std::string &date_str) {
    std::tm date = {};
    std::istringstream ss(date_str);
    ss >> date.tm_year;
    ss.ignore();
    ss >> date.tm_mon;
    ss.ignore();
    ss >> date.tm_mday;

    auto now = std::chrono::system_clock::now();
    auto currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm current = *std::localtime(&currentTime);

    current.tm_year -= 18 - 1900;
    return std::mktime(&date) <= std::mktime(&current);
}

bool Validator::validDate(const std::string &date_str) {
    if (!std::regex_match(date_str, date_pattern)) {
        return false;
    }

    std::tm date = {};
    std::istringstream ss(date_str);
    ss >> date.tm_year;
    ss.ignore();
    ss >> date.tm_mon;
    ss.ignore();
    ss >> date.tm_mday;

    int maxDay;
    switch (date.tm_mon) {
        case 2:
            maxDay = (date.tm_year % 4 == 0 && (date.tm_year % 100 != 0 || date.tm_year % 400 == 0)) ? 29 : 28;
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            maxDay = 30;
            break;
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            maxDay = 31;
            break;
        default:
            return false;
    }
    if (date.tm_mday > maxDay || date.tm_mday < 1) {
        return false;
    }

    return true;
}

bool Validator::validDriver(const Driver &driver) {
    if (!std::regex_match(driver.getLogin(), login_pattern)) {
        throw std::invalid_argument("Login must contains only latin symbols, digits or _");
    }
    if (driver.getPassHash().empty()) {
        throw std::invalid_argument("Password must not be empty");
    }
    if (!std::regex_match(driver.getName(), name_pattern)) {
        throw std::invalid_argument("Name must consist only of latin characters");
    }
    if (!std::regex_match(driver.getSurname(), name_pattern)) {
        throw std::invalid_argument("Surname must consist only of latin characters");
    }
    if (driver.getExperience() < 0) {
        throw std::invalid_argument("Experience cannot be negative");
    }

    if (!std::regex_match(driver.getAddress(), address_pattern)) {
        throw std::invalid_argument("Address must consist only of latin characters");
    }

    if (!std::regex_match(driver.getCity(), city_pattern)) {
        throw std::invalid_argument("City must consist only of latin characters");
    }

    if (!std::regex_match(driver.getBirthday(), date_pattern)) {
        throw std::invalid_argument("Birthday must satisfy format YYYY-MM-DD");
    }

    if (!validAge(driver.getBirthday())) {
        throw std::invalid_argument("Age must be >= 18 years");
    }

    return true;
}

bool Validator::validOrder(const Order &order, sqlite3 *db) {
    Driver d;
    Car c;
    try {
        d.getDataFromDb(db, order.getDriverId());
    } catch (const std::exception &e) {
        throw std::invalid_argument("Driver doesn't exists");
    }

    try {
        c.getDataFromDb(db, order.getCarId());
    } catch (const std::exception &e) {
        throw std::invalid_argument("Car doesn't exists");
    }

    if (c.getDriverId() != d.getId()) {
        throw std::invalid_argument("Driver doesn't obtain the car");
    }

    if (c.getLoadCapacity() < order.getLoad()) {
        throw std::invalid_argument("Load of cargo > load capacity of the car");
    }

    if (!std::regex_match(order.getDate(), date_pattern)) {
        throw std::invalid_argument("Date does not satisfy pattern YYYY-MM-DD");
    }

    if (!validDate(order.getDate())) {
        throw std::invalid_argument("Invalid date was provided");
    }

    if (order.getMileage() < 0) {
        throw std::invalid_argument("Mileage cannot be negative");
    }

    if (order.getCost() < 0) {
        throw std::invalid_argument("Cost cannot be negative");
    }

    if (order.getLoad() < 0) {
        throw std::invalid_argument("Load cannot be negative");
    }

    return true;
}

bool Validator::validDispatcher(const Dispatcher &dispatcher) {

    if (dispatcher.getLogin().empty()) {
        throw std::invalid_argument("Login must not be empty");
    }

    if (dispatcher.getPassHash().empty()) {
        throw std::invalid_argument("Password must not be empty");
    }

    if (!std::regex_match(dispatcher.getName(), name_pattern)) {
        throw std::invalid_argument("Name must consist only of latin characters");
    }

    if (!std::regex_match(dispatcher.getSurname(), name_pattern)) {
        throw std::invalid_argument("Surname must consist only of latin characters");
    }

    if (!std::regex_match(dispatcher.getAddress(), address_pattern)) {
        throw std::invalid_argument("Address must consist only of latin characters");
    }

    if (!std::regex_match(dispatcher.getCity(), city_pattern)) {
        throw std::invalid_argument("City must consist only of latin characters");
    }

    return true;
}

bool Validator::validCar(const Car &car, sqlite3 *db) {
    try {
        Driver d;
        d.getDataFromDb(db, car.getDriverId());
    } catch (const std::exception &e) {
        throw std::invalid_argument("There is no driver to own this car");
    }

    if (!Validator::validLicense(car.getLicense())) {
        throw std::invalid_argument("Invalid car license");
    }

    if (car.getMileageBuy() < 0) {
        throw std::invalid_argument("Invalid car mileage");
    }

    if (car.getLoadCapacity() < 0) {
        throw std::invalid_argument("Invalid car mileage");
    }

    return true;
}

void Validator::validateUpdateCar(Car &update, int car_id, sqlite3 *db) {
    Car old;

    try {
        old.getDataFromDb(db, car_id);
    } catch (const std::exception &e) {
        throw std::invalid_argument("No car found by provided id");
    }

    if (update.getLicense().empty()) {
        update.setLicense(old.getLicense());
    }

    if (update.getBrand().empty()) {
        update.setBrand(old.getBrand());
    }

    if (update.getDriverId() == 0) {
        update.setDriverId(old.getDriverId());
    }

    if (update.getLoadCapacity() == 0) {
        update.setLoadCapacity(old.getLoadCapacity());
    }

    if (update.getMileageBuy() == 0) {
        update.setMileageBuy(old.getMileageBuy());
    }

    Validator::validCar(update, db);
}

void Validator::validateUpdateDriver(Driver &update, int driver_id, sqlite3 *db) {
    Driver old;
    try {
        old.getDataFromDb(db, driver_id);
    } catch (const std::exception &e) {
        throw std::invalid_argument("No driver found by provided id");
    }

    update.setLogin("TEMP");
    update.setPassHash("TEMP");

    if (update.getName().empty()) {
        update.setName(old.getName());
    }

    if (update.getSurname().empty()) {
        update.setSurname(old.getSurname());
    }

    if (update.getCategoryString().empty()) {
        update.setCategory(old.getCategories());
    }

    if (update.getExperience() == 0) {
        update.setExperience(old.getExperience());
    }

    if (update.getAddress().empty()) {
        update.setAddress(old.getAddress());
    }

    if (update.getCity().empty()) {
        update.setCity(old.getCity());
    }

    if (update.getBirthday().empty()) {
        update.setBirthday(old.getBirthday());
    }

    Validator::validDriver(update);
}

void Validator::validateUpdateDispatcher(Dispatcher &update, int dispatcher_id, sqlite3 *db) {
    Dispatcher old;
    try {
        old.getDataFromDb(db, dispatcher_id);
    } catch (const std::exception &e) {
        throw std::invalid_argument("No dispatcher found by provided id");
    }

    update.setLogin("TEMP");
    update.setPassHash("TEMP");

    if (update.getName().empty()) {
        update.setName(old.getName());
    }

    if (update.getSurname().empty()) {
        update.setSurname(old.getSurname());
    }

    if (update.getAddress().empty()) {
        update.setAddress(old.getAddress());
    }

    if (update.getCity().empty()) {
        update.setCity(old.getCity());
    }


    Validator::validDispatcher(update);
}

void Validator::validateUpdateOrder(Order &update, int order_id, sqlite3 *db) {
    Order old;
    try {
        old.getDataFromDb(db, order_id);
    } catch (const std::exception &e) {
        throw std::invalid_argument("No order found by provided id");
    }

    if (update.getDriverId() == 0) {
        update.setDriverId(old.getDriverId());
    }

    if (update.getCarId() == 0) {
        update.setCarId(old.getCarId());
    }

    if (update.getDate().empty()) {
        update.setDate(old.getDate());
    }

    if (update.getMileage() == 0.) {
        update.setMileage(old.getMileage());
    }

    if (update.getLoad() == 0.) {
        update.setLoad(old.getLoad());
    }

    if (update.getCost() == 0.) {
        update.setCost(old.getCost());
    }

    Validator::validOrder(update, db);
}

void Validator::validateUpdateUser(User &update, int user_id, sqlite3 *db) {
    User old;
    try {
        old.getDataFromDb(db, user_id);
    } catch (const std::exception &c) {
        throw std::invalid_argument("No user found by provided id");
    }

    if (update.getLogin().empty()) {
        update.setLogin(old.getLogin());
    }

    if (update.getPassHash().empty()) {
        update.setPassHash(old.getPassHash());
    } else {
        update.setPassHash(hashPassword(update.getPassHash()));
    }
}

std::string Validator::hashPassword(const std::string &password)
{
    /*
        EVP is openssl high-level interface I use for digest(hash) a password
        It requires EVP_DigestInit, EVP_DigestUpdate and EVP_DigestFinal
        The result is put into hashed variable
    */
    EVP_MD_CTX *context = EVP_MD_CTX_new();

    std::string hashed;

    if (context != nullptr)
    {
        if (EVP_DigestInit_ex(context, EVP_sha256(), nullptr))
        {
            if (EVP_DigestUpdate(context, password.c_str(), password.length()))
            {
                unsigned char hash[EVP_MAX_MD_SIZE];
                unsigned int lengthOfHash = 0;

                if (EVP_DigestFinal_ex(context, hash, &lengthOfHash))
                {
                    std::stringstream ss;
                    for (unsigned int i = 0; i < lengthOfHash; ++i)
                    {
                        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
                    }

                    hashed = ss.str();
                }
            }
        }

        EVP_MD_CTX_free(context);
    }
    return hashed;
}

bool Validator::validatePassword(const std::string& hash_pass, const std::string& provided_pass) {
    std::string provided_hash = hashPassword(provided_pass);
    return hash_pass == provided_hash;
}