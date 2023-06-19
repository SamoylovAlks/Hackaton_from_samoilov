#include <iostream>
#include <thread>
#include <condition_variable>
#include <random>
#include <chrono>

enum Ingredient {
    UNICORN_HAIR,
    PHOENIX_FEATHER,
    DRAGON_SCALE
};

class PotionMaster {
    std::string name;
    Ingredient ingredient;

public:
    PotionMaster(std::string n, Ingredient i) : name(n), ingredient(i) {}

    Ingredient get_ingredient() { return ingredient; }

    std::string get_name() { return name; }

    void prepare_potion(Ingredient first, Ingredient second) {
        std::cout << name << " is preparing a potion with " << to_string(first) << " and " << to_string(second) << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << name << " has finished the potion.\n" << std::endl;
    }

    std::string to_string(Ingredient ingredient) {
        switch (ingredient) {
        case UNICORN_HAIR:
            return "Unicorn Hair";
        case PHOENIX_FEATHER:
            return "Phoenix Feather";
        case DRAGON_SCALE:
            return "Dragon Scale";
        default:
            return "Unknown ingredient";
        }
    }
};

class MagicalCreature {
public:
    std::pair<Ingredient, Ingredient> put_ingredients() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(0, 2);

        Ingredient first = static_cast<Ingredient>(distr(gen));
        Ingredient second;
        do {
            second = static_cast<Ingredient>(distr(gen));
        } while (second == first);

        std::cout << "Magical Creature put " << to_string(first) << " and " << to_string(second) << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return {first, second};
    }

    std::string to_string(Ingredient ingredient) {
        switch (ingredient) {
        case UNICORN_HAIR:
            return "Unicorn Hair";
        case PHOENIX_FEATHER:
            return "Phoenix Feather";
        case DRAGON_SCALE:
            return "Dragon Scale";
        default:
            return "Unknown ingredient";
        }
    }
};

int main() {
    std::array<PotionMaster, 3> masters = {
        PotionMaster("Master_Unicorn_Hair", UNICORN_HAIR),
        PotionMaster("Master_Phoenix_Feather", PHOENIX_FEATHER),
        PotionMaster("Master_Dragon_Scale", DRAGON_SCALE)
    };

    MagicalCreature creature;

    std::mutex mtx;
    std::condition_variable cv;
    Ingredient table[2];
    bool ready = false;

    std::thread magical_creature_thread;
    std::thread potion_master_threads[3];
    
    for (int i = 0; i < 3; i++) {
        potion_master_threads[i] = std::thread([&masters, &table, &mtx, &ready, &cv, i]() {
            while (true) {
                std::unique_lock<std::mutex> lock(mtx);
                cv.wait(lock, [&]() { return ready && (masters[i].get_ingredient() != table[0] && masters[i].get_ingredient() != table[1]); });
                masters[i].prepare_potion(table[0], table[1]);
                ready = false;
                cv.notify_all();
            }
        });
    }

    magical_creature_thread = std::thread([&]() {
        while (true) {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]() { return !ready; });
            auto [first, second] = creature.put_ingredients();
            table[0] = first;
            table[1] = second;
            ready = true;
            cv.notify_all();
        }
    });

    magical_creature_thread.join();
    for (auto& thread : potion_master_threads) {
        thread.join();
    }

    return 0;
}
