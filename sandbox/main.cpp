#include <spdlog/spdlog.h>
#include "reflection/refl.h"

struct Person
{
    short age;
    int id;
    enum class Gender
    {
        Male,
        Female
    } gender;
    Person() = default;
    Person(const Person &other) : age(age), id(id) { spdlog::info("person copied"); }
    Person(Person &&other) : age(std::move(age)), id(std::move(id)) { spdlog::info("person moved"); }
    ~Person() { spdlog::info("person released"); }
};

int main(int argc, char **argv)
{
    Reflection::Register<Person>("Person") //
        .AddProperty(&Person::age, "age")
        .AddProperty(&Person::id, "id");

    Reflection::Register<Person::Gender>("Person::Gender")
        .Add("Male", Person::Gender::Male)
        .Add("Female", Person::Gender::Female);

    using namespace Reflection;
    {
        Person p;
        p.age = 18;
        p.id = 123456;
        auto properties = GetType<Person>()->As<Class>()->GetProperties();
        for (const auto &prop : properties)
        {
            auto per = MakeRef(p);
            auto numType = prop->GetTypeInfo()->As<Numeric>();
            spdlog::info("{}", numType->GetName());
            auto member = prop->Call(per);
            numType->SetValue(19, member);
        }
        spdlog::info("{}, {}", p.age, p.id);
    }

    return 0;
}