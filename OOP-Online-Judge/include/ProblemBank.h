#ifndef OJ_PROBLEMBANK_H
#define OJ_PROBLEMBANK_H

#include "Problem.h"
#include "TextUtil.h"

namespace OJ {

class ProblemBank {
    Problem* problems[500];
    int problemCount;

    static bool sameName(const char* a, const char* b) {
        if (a == b) return true;
        if (!a || !b) return false;
        int i = 0;
        while (a[i] != '\0' && b[i] != '\0') {
            if (a[i] != b[i]) return false;
            i++;
        }
        return a[i] == '\0' && b[i] == '\0';
    }

    static Directory* findChildDirByName(Directory* parent, const char* name) {
        if (!parent) return NULL;
        for (int i = 0; i < parent->childCount(); i++) {
            Directory* d = (Directory*)(parent->childAt(i));
            if (d && sameName(d->getName(), name)) return d;
        }
        return NULL;
    }

    static void addShortAnswerProblem(VFSManager& vfs, Directory* root, int id, int diff,
                                      const char* title, const char* statement, const char* answer) {
        Problem p(id, diff, Problem::ShortAnswer, title, statement, "", "");
        p.addTestCase(TestCase("", answer));
        p.saveToVFS(vfs, root);
    }

public:
    ProblemBank() : problemCount(0) {
        for (int i = 0; i < 500; i++) problems[i] = NULL;
    }

    ~ProblemBank() {
        clear();
    }

    void clear() {
        for (int i = 0; i < problemCount; i++) {
            if (problems[i]) delete problems[i];
            problems[i] = NULL;
        }
        problemCount = 0;
    }

    int count() const { return problemCount; }

    Problem* getAt(int index) const {
        if (index < 0 || index >= problemCount) return NULL;
        return problems[index];
    }

    void addProblem(Problem* p) {
        if (!p) return;
        if (problemCount < 500) {
            problems[problemCount++] = p;
        }
    }

    Problem* getById(int id) const {
        for (int i = 0; i < problemCount; i++) {
            if (problems[i] && problems[i]->getId() == id) return problems[i];
        }
        return NULL;
    }

    void filterByDifficulty(int diff, Problem** out, int& outCount, int maxOut) const {
        outCount = 0;
        for (int i = 0; i < problemCount; i++) {
            if (problems[i] && problems[i]->getDifficulty() == diff) {
                if (outCount < maxOut) {
                    out[outCount++] = problems[i];
                }
            }
        }
    }

    // Ensure /problems exists and seed the OOP short-answer question bank.
    Directory* seedIfMissing(VFSManager& vfs) {
        Directory* root = vfs.getRoot();
        if (!root) return NULL;

        Directory* problemsRoot = findChildDirByName(root, "problems");
        if (problemsRoot != NULL) return problemsRoot;

        problemsRoot = vfs.createDirectory(root, "problems");
        if (!problemsRoot) return NULL;

        addShortAnswerProblem(vfs, problemsRoot, 1, 1, "MissingSemicolon",
            "cout << \"Hello World\"\n\nWhat is missing?", ";");
        addShortAnswerProblem(vfs, problemsRoot, 2, 1, "ClassEndSymbol",
            "class Student\n{\n};\n\nWhat symbol ends the class?", ";");
        addShortAnswerProblem(vfs, problemsRoot, 3, 1, "ObjectDeclaration",
            "Student s1;\n\nWhat is s1?", "Object");
        addShortAnswerProblem(vfs, problemsRoot, 4, 1, "PublicKeyword",
            "class Car\n{\npublic:\nint speed;\n};\n\nWhich keyword makes members accessible outside class?", "public");
        addShortAnswerProblem(vfs, problemsRoot, 5, 1, "PrivateAccess",
            "class Test\n{\nprivate:\nint x;\n};\n\nCan x be accessed directly outside class?", "No");
        addShortAnswerProblem(vfs, problemsRoot, 6, 1, "ConstructorCallTime",
            "class A\n{\npublic:\nA()\n{\ncout << \"Constructor\";\n}\n};\n\nWhen is constructor called?", "When object is created");
        addShortAnswerProblem(vfs, problemsRoot, 7, 1, "ConstructorRuns",
            "A a1;\nA a2;\n\nHow many times constructor runs?", "2 times");
        addShortAnswerProblem(vfs, problemsRoot, 8, 1, "DestructorFunction",
            "class A\n{\npublic:\n~A()\n{\ncout << \"Destroyed\";\n}\n};\n\nWhat type of function is ~A()?", "Destructor");
        addShortAnswerProblem(vfs, problemsRoot, 9, 1, "NewMemoryLocation",
            "int* p = new int;\n\nWhere is memory allocated?", "Heap memory");
        addShortAnswerProblem(vfs, problemsRoot, 10, 1, "DeletePurpose",
            "delete p;\n\nWhy is delete used?", "To free dynamic memory");

        addShortAnswerProblem(vfs, problemsRoot, 11, 2, "DataMember",
            "class Student\n{\npublic:\nstring name;\n};\n\nWhat is name called?", "Data member / Attribute");
        addShortAnswerProblem(vfs, problemsRoot, 12, 2, "MemberFunction",
            "void display()\n{\ncout << \"Hi\";\n}\n\nInside class, what is this called?", "Member function");
        addShortAnswerProblem(vfs, problemsRoot, 13, 2, "DotOperator",
            "Student s1;\ns1.name = \"Ali\";\n\nWhat operator is used to access member?", "Dot (.) operator");
        addShortAnswerProblem(vfs, problemsRoot, 14, 2, "ArrowOperator",
            "Student* s = new Student;\ns->name = \"Ali\";\n\nWhich operator is used?", "Arrow (->) operator");
        addShortAnswerProblem(vfs, problemsRoot, 15, 2, "CopyConstructor",
            "A(const A& obj)\n{\n}\n\nWhich constructor is this?", "Copy constructor");
        addShortAnswerProblem(vfs, problemsRoot, 16, 2, "ShallowCopyProblem",
            "What problem happens in shallow copy?", "Multiple objects share same memory");
        addShortAnswerProblem(vfs, problemsRoot, 17, 2, "DeepCopy",
            "What solves shallow copy issue?", "Deep copy");
        addShortAnswerProblem(vfs, problemsRoot, 18, 2, "StaticKeyword",
            "Which keyword stores common data for all objects?", "static");
        addShortAnswerProblem(vfs, problemsRoot, 19, 2, "StaticShared",
            "static int count;\n\nIs this shared by all objects?", "Yes");
        addShortAnswerProblem(vfs, problemsRoot, 20, 2, "OOPFullForm",
            "What does OOP stand for?", "Object Oriented Programming");

        addShortAnswerProblem(vfs, problemsRoot, 21, 3, "OOPPillars",
            "Name the 4 pillars of OOP.", "Encapsulation Abstraction Inheritance Polymorphism");
        addShortAnswerProblem(vfs, problemsRoot, 22, 3, "AnimalSoundFunction",
            "class Animal\n{\npublic:\nvoid sound()\n{\ncout << \"Animal Sound\";\n}\n};\n\nWhat is sound() here?", "Member function");
        addShortAnswerProblem(vfs, problemsRoot, 23, 3, "InheritanceRelation",
            "class Dog : public Animal\n{\n};\n\nWhat relationship is this?", "Inheritance");
        addShortAnswerProblem(vfs, problemsRoot, 24, 3, "ParentClass",
            "Which class is parent class here?\n\nclass Dog : public Animal", "Animal");
        addShortAnswerProblem(vfs, problemsRoot, 25, 3, "ChildClass",
            "Which class is child class here?\n\nclass Dog : public Animal", "Dog");
        addShortAnswerProblem(vfs, problemsRoot, 26, 3, "FunctionOverloading",
            "What is function overloading?", "Same function name with different parameters");
        addShortAnswerProblem(vfs, problemsRoot, 27, 3, "OperatorOverloading",
            "What is operator overloading?", "Giving special meaning to operators for objects");
        addShortAnswerProblem(vfs, problemsRoot, 28, 3, "PlusOverload",
            "Complex operator+(Complex c)\n\nWhich operator is overloaded?", "+");
        addShortAnswerProblem(vfs, problemsRoot, 29, 3, "OutputOperator",
            "Which operator is overloaded for output?", "<<");
        addShortAnswerProblem(vfs, problemsRoot, 30, 3, "InputOperator",
            "Which operator is overloaded for input?", ">>");

        addShortAnswerProblem(vfs, problemsRoot, 31, 4, "PointerStores",
            "int* ptr;\n\nWhat does ptr store?", "Address");
        addShortAnswerProblem(vfs, problemsRoot, 32, 4, "AddressOfOperator",
            "ptr = &x;\n\nWhat does & mean?", "Address of operator");
        addShortAnswerProblem(vfs, problemsRoot, 33, 4, "DereferenceOperator",
            "cout << *ptr;\n\nWhat does * do here?", "Dereference operator");
        addShortAnswerProblem(vfs, problemsRoot, 34, 4, "ArrayDelete",
            "delete[] arr;\n\nWhy [] is used?", "Array memory deletion");
        addShortAnswerProblem(vfs, problemsRoot, 35, 4, "MemoryLeak",
            "What happens if dynamic memory is not deleted?", "Memory leak");
        addShortAnswerProblem(vfs, problemsRoot, 36, 4, "Aggregation",
            "A university has departments. Departments can exist separately.\n\nWhich relationship?", "Aggregation");
        addShortAnswerProblem(vfs, problemsRoot, 37, 4, "Composition",
            "A house has rooms. If house is destroyed, rooms are also destroyed.\n\nWhich relationship?", "Composition");
        addShortAnswerProblem(vfs, problemsRoot, 38, 4, "Association",
            "Teacher teaches Student.\n\nWhich relationship?", "Association");
        addShortAnswerProblem(vfs, problemsRoot, 39, 4, "StrongestRelationship",
            "Which relationship is strongest?", "Composition");
        addShortAnswerProblem(vfs, problemsRoot, 40, 4, "WeakestRelationship",
            "Which relationship is weakest?", "Association");

        addShortAnswerProblem(vfs, problemsRoot, 41, 5, "VirtualFunction",
            "virtual void display();\n\nWhat does virtual enable?", "Runtime polymorphism");
        addShortAnswerProblem(vfs, problemsRoot, 42, 5, "RuntimePolymorphism",
            "Animal* a = new Dog();\n\nWhat type of polymorphism is this?", "Runtime polymorphism");
        addShortAnswerProblem(vfs, problemsRoot, 43, 5, "DynamicBinding",
            "Why do we use virtual functions?", "To achieve dynamic binding");
        addShortAnswerProblem(vfs, problemsRoot, 44, 5, "CastingOperator",
            "Directory* d = (Directory*)(arr[i]);\n\nWhat does (Directory*) do?", "Converts base pointer to derived pointer");
        addShortAnswerProblem(vfs, problemsRoot, 45, 5, "FinalKeyword",
            "Which keyword prevents inheritance?", "final");
        addShortAnswerProblem(vfs, problemsRoot, 46, 5, "Abstraction",
            "What is abstraction?", "Hiding implementation details");
        addShortAnswerProblem(vfs, problemsRoot, 47, 5, "Encapsulation",
            "What is encapsulation?", "Wrapping data and functions together");
        addShortAnswerProblem(vfs, problemsRoot, 48, 5, "DestructorScope",
            "Which function is automatically called when object goes out of scope?", "Destructor");
        addShortAnswerProblem(vfs, problemsRoot, 49, 5, "UMLFullForm",
            "What is UML?", "Unified Modeling Language");
        addShortAnswerProblem(vfs, problemsRoot, 50, 5, "UMLDiamond",
            "Which UML relationship uses diamond symbol?", "Aggregation -> Empty diamond Composition -> Filled diamond");

        return problemsRoot;
    }

    bool loadFromVFS(VFSManager& vfs) {
        clear();
        Directory* root = vfs.getRoot();
        if (!root) return false;

        Directory* problemsRoot = findChildDirByName(root, "problems");
        if (!problemsRoot) return false;

        for (int i = 0; i < problemsRoot->childCount(); i++) {
            Directory* dDir = (Directory*)(problemsRoot->childAt(i));
            if (!dDir) continue;

            for (int j = 0; j < dDir->childCount(); j++) {
                Directory* pDir = (Directory*)(dDir->childAt(j));
                if (!pDir) continue;

                Problem temp;
                if (Problem::loadFromVFS(pDir, temp)) {
                    Problem* heap = new Problem(temp);
                    addProblem(heap);
                }
            }
        }

        return true;
    }
};

}

#endif
