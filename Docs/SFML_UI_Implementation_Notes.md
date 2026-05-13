# SFML UI Implementation Notes

This file explains how to build the final simple UI design in SFML for the existing OOP Online Judge project.

Design preview file:

- `Raw/SFML_Simple_UI_Design.html`

The design must stay simple because SFML is not a web UI framework. Use fixed positions, rectangles, text, and simple mouse/keyboard events.

## 1. Main Idea

Use one SFML window and switch between screens.

Window:

```cpp
sf::RenderWindow window(sf::VideoMode(800, 520), "OOP Online Judge");
```

Screens:

```cpp
enum ScreenType {
    SCREEN_LOGIN,
    SCREEN_REGISTER,
    SCREEN_DASHBOARD,
    SCREEN_PROBLEM_BANK,
    SCREEN_CONTEST,
    SCREEN_RESULT
};
```

Only draw and update the active screen.

## 2. Files To Add

Create a new folder:

```text
OOP-Online-Judge/sfml_ui/
```

Add these files:

```text
sfml_ui/
  SFMLApp.h
  SFMLApp.cpp
  UITheme.h
  UITheme.cpp
  UIButton.h
  UIButton.cpp
  UITextInput.h
  UITextInput.cpp
  UISidebar.h
  UISidebar.cpp
  LoginView.h
  LoginView.cpp
  RegisterView.h
  RegisterView.cpp
  DashboardView.h
  DashboardView.cpp
  ProblemBankView.h
  ProblemBankView.cpp
  ContestView.h
  ContestView.cpp
  ResultView.h
  ResultView.cpp
```

Keep the old console files. The SFML UI should call the existing project classes like `UserManager`, `ProblemBank`, `ContestManager`, `EvaluationEngine`, and `Session`.

## 3. Existing Project Files Used

The SFML UI should use these existing files:

```text
include/UserManager.h
include/Session.h
include/ProblemBank.h
include/Problem.h
include/ContestManager.h
include/Contest.h
include/EvaluationEngine.h
include/Verdict.h
include/VFSManager.h
include/ContestResult.h
```

Do not rewrite the logic. SFML should only replace console input/output.

## 4. UITheme.h

Use one place for colors and sizes.

```cpp
#ifndef OJ_UITHEME_H
#define OJ_UITHEME_H

#include <SFML/Graphics.hpp>

namespace OJUI {

struct UITheme {
    static sf::Color Background();
    static sf::Color TitleBar();
    static sf::Color Panel();
    static sf::Color Border();
    static sf::Color Button();
    static sf::Color ButtonDark();
    static sf::Color ButtonGray();
    static sf::Color Danger();
    static sf::Color Text();
    static sf::Color MutedText();
    static sf::Color Error();
    static sf::Color Success();
};

}

#endif
```

Colors to use:

```text
Background: #f4f6fa
TitleBar/sidebar: #111827
Panel: #ffffff
Border: #c5ccd6
Button: #2563eb
Danger: #b91c1c
Error: #dc2626
Success: #166534
```

## 5. UIButton

Use this for every button.

Files:

```text
sfml_ui/UIButton.h
sfml_ui/UIButton.cpp
```

Class idea:

```cpp
class UIButton {
private:
    sf::RectangleShape box;
    sf::Text label;

public:
    UIButton();
    void setup(const sf::Font& font, const char* text, float x, float y, float w, float h, sf::Color color);
    void draw(sf::RenderWindow& window) const;
    bool contains(sf::Vector2f mousePos) const;
};
```

Use for:

- Login button
- Register button
- Register Account button
- Filter Difficulty button
- Submit Answer button
- Save Back button
- End Contest button
- Dashboard buttons

Memory note:

- Store `UIButton` objects as direct members, not pointers.
- Do not create/destroy buttons every frame.

## 6. UITextInput

Use this for username, email, password, difficulty, and answer input.

Files:

```text
sfml_ui/UITextInput.h
sfml_ui/UITextInput.cpp
```

Class idea:

```cpp
class UITextInput {
private:
    sf::RectangleShape box;
    sf::Text text;
    bool focused;
    char buffer[256];
    int length;

public:
    UITextInput();
    void setup(const sf::Font& font, float x, float y, float w, float h);
    void setFocused(bool value);
    void clear();
    void setText(const char* value);
    const char* getText() const;
    void handleTextEntered(sf::Uint32 unicode);
    void draw(sf::RenderWindow& window) const;
    bool contains(sf::Vector2f mousePos) const;
};
```

Important:

- Use fixed `char buffer[256]` to manage memory simply.
- Do not use dynamic memory for text input.
- Backspace should remove one character.
- Ignore non-printable characters except backspace.

## 7. UISidebar

Use this for Dashboard, Problem Bank, Contest, and Result screens.

Files:

```text
sfml_ui/UISidebar.h
sfml_ui/UISidebar.cpp
```

Draw:

- Dark rectangle at `x=0`, `y=52`, `w=180`, `h=468`
- Brand text: `OOP Judge`
- Nav items:
  - Dashboard
  - Problem Bank
  - Contest
  - Results
  - Logout

Use `sf::RectangleShape` for nav item boxes and `sf::Text` for labels.

## 8. SFMLApp

Files:

```text
sfml_ui/SFMLApp.h
sfml_ui/SFMLApp.cpp
```

This class controls the app.

It should own:

```cpp
sf::RenderWindow window;
sf::Font font;
ScreenType currentScreen;

VFSManager* vfs;
OJ::UserManager* userManager;
OJ::Session session;
OJ::ProblemBank problemBank;
OJ::ContestManager* contestManager;
OJ::EvaluationEngine engine;
```

It should also own each screen:

```cpp
LoginView loginView;
RegisterView registerView;
DashboardView dashboardView;
ProblemBankView problemBankView;
ContestView contestView;
ResultView resultView;
```

Memory rule:

- Prefer direct members.
- `ContestManager` can be a pointer because it depends on `ProblemBank`, but delete it in destructor.
- Load the font once in `SFMLApp::init()`.

Main loop:

```cpp
void SFMLApp::run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            handleEvent(event);
        }

        update();

        window.clear(UITheme::Background());
        draw();
        window.display();
    }
}
```

## 9. main.cpp Changes

In `OOP-Online-Judge/main.cpp`, keep the console mode if needed, but add SFML mode.

Add include:

```cpp
#include "sfml_ui/SFMLApp.h"
```

Change `main()` idea:

```cpp
int main(int argc, char** argv) {
    if (argc >= 2 && sameName(argv[1], "--sfml")) {
        VFSManager vfs;
        vfs.init("OJ_VOL");

        OJ::UserManager userManager(vfs);

        OJUI::SFMLApp app(vfs, userManager);
        app.run();
        return 0;
    }

    if (argc >= 2 && (sameName(argv[1], "--demo") || sameName(argv[1], "demo") || sameName(argv[1], "--phase8"))) {
        return runDemoPhase8();
    }

    return runUI();
}
```

Then run:

```powershell
.\main.exe --sfml
```

## 10. LoginView

Files:

```text
sfml_ui/LoginView.h
sfml_ui/LoginView.cpp
```

Draw these:

- Title bar: `Login`
- Main panel: `x=230`, `y=105`, `w=340`, `h=330`
- Heading: `OOP Online Judge`
- Username input
- Password input
- Login button
- Register button
- Error text if login fails

Use:

```cpp
UserManager::login(username, password, session)
```

On Login button:

- If login succeeds, switch to `SCREEN_DASHBOARD`.
- If wrong password/user not found/not verified, show error text.

On Register button:

- Switch to `SCREEN_REGISTER`.

## 11. RegisterView

Files:

```text
sfml_ui/RegisterView.h
sfml_ui/RegisterView.cpp
```

Draw these:

- Title bar: `Register`
- Main panel: `x=210`, `y=78`, `w=380`, `h=385`
- Username input
- Email input
- Password input
- Register Account button
- Error text

Register behavior:

1. User enters username.
2. If username already exists, show `User already exists`.
3. User enters email.
4. If email does not end with `@email.com`, show `Email must end with @email.com`.
5. If email already exists, show `Email already exists`.
6. Only then allow password.
7. On Register Account, call:

```cpp
CustomString token;
userManager.registerUser(username, email, password, token);
userManager.verifyUser(username, token.c_str());
```

For simple SFML version, you can auto-verify after register, or show a small token input screen later. If following current console logic exactly, add another `UITextInput tokenInput`.

Use:

```cpp
userManager.usernameExists(username)
userManager.emailExists(email)
```

## 12. DashboardView

Files:

```text
sfml_ui/DashboardView.h
sfml_ui/DashboardView.cpp
```

Draw:

- Title bar: `Dashboard`
- Sidebar
- Main panel: `x=210`, `y=82`, `w=550`, `h=390`
- Four menu boxes:
  - Browse Problems
  - Start Contest
  - Resume Contest
  - View Result
- Active contest scorebar

Button behavior:

- Browse Problems -> `SCREEN_PROBLEM_BANK`
- Start Contest -> create contest then `SCREEN_CONTEST`
- Resume Contest -> resume contest then `SCREEN_CONTEST`
- View Result -> `SCREEN_RESULT`
- Logout -> end session and go to `SCREEN_LOGIN`

## 13. ProblemBankView

Files:

```text
sfml_ui/ProblemBankView.h
sfml_ui/ProblemBankView.cpp
```

Draw:

- Title bar: `Problem Bank`
- Sidebar
- Left panel: `x=205`, `y=78`, `w=235`, `h=390`
- Difficulty input box
- Filter Difficulty button
- Problem list showing only 4 rows
- Scrollbar rectangle for remaining problems
- Right panel: `x=460`, `y=78`, `w=300`, `h=390`
- Selected problem title
- Statement box
- Answer input
- Submit Answer button
- Verdict and credit text

Problem list behavior:

- Store `selectedDifficulty`.
- Store `scrollIndex`.
- Show only 4 problems:

```cpp
for (int i = 0; i < 4; i++) {
    int problemIndex = scrollIndex + i;
}
```

Mouse wheel:

- If wheel up, decrease `scrollIndex`.
- If wheel down, increase `scrollIndex`.
- Clamp between `0` and `filteredCount - 4`.

Filter:

```cpp
problemBank.filterByDifficulty(diff, filteredProblems);
scrollIndex = 0;
```

Submit answer:

```cpp
Verdict v = engine.evaluateAnswer(answerInput.getText(), *selectedProblem);
```

If accepted:

```cpp
credit = selectedProblem->getDifficulty() * 100;
```

## 14. ContestView

Files:

```text
sfml_ui/ContestView.h
sfml_ui/ContestView.cpp
```

Draw:

- Title bar: `Contest`
- Sidebar
- Main panel: `x=205`, `y=78`, `w=555`, `h=390`
- Scorebar:
  - Time
  - Solved
  - Credit
  - Score
- Text:
  - `Contest uses a real countdown. When time ends, result is saved and the screen returns to Dashboard.`
- Current question statement
- Answer input
- Buttons at bottom:
  - Submit
  - Save Back
  - End Contest

Timer behavior:

- Every frame call:

```cpp
activeContest->remainingSeconds()
```

- If remaining time is `0`:

```cpp
contestManager->endContest(session, true);
currentScreen = SCREEN_DASHBOARD;
```

Submit behavior:

```cpp
Verdict v = engine.evaluateAnswer(answerInput.getText(), *problem);
bool accepted = (v.getCode() == Verdict::Accepted);
activeContest->recordAttempt(index, accepted, answerInput.getText());
activeContest->saveProgress(vfs, userDir);
```

Score text:

- Use same score logic from `ContestScreen.cpp`.
- Show:
  - solved count
  - wrong attempts
  - credit
  - current score

Memory note:

- `activeContest` may be a pointer returned by `ContestManager`.
- Delete it when leaving contest if no longer needed.
- If saving and going back, save first, then delete local pointer.

## 15. ResultView

Files:

```text
sfml_ui/ResultView.h
sfml_ui/ResultView.cpp
```

Draw:

- Title bar: `Result`
- Main panel: `x=180`, `y=95`, `w=440`, `h=330`
- Score
- Contest type
- Solved
- Wrong attempts
- Minutes used
- Dashboard button
- Problems button

Use existing result VFS reading logic from `ResultScreen.cpp`.

You can move shared result-loading helper logic into a reusable function if needed.

## 16. Build Command

If SFML is installed, add SFML files to compile command.

Example:

```powershell
g++ -std=c++17 -Iinclude -Isfml_ui -o main.exe main.cpp `
  src/CustomString.cpp src/User.cpp src/Session.cpp src/VerificationService.cpp src/UserManager.cpp `
  src/TestCase.cpp src/Problem.cpp src/ProblemBank.cpp src/Submission.cpp src/Verdict.cpp `
  src/OutputComparator.cpp src/EvaluationEngine.cpp src/ScoreCalculator.cpp src/ContestResult.cpp `
  src/UIManager.cpp src/LoginScreen.cpp src/RegisterScreen.cpp src/DashboardScreen.cpp `
  src/ProblemBrowserScreen.cpp src/ContestScreen.cpp src/ResultScreen.cpp `
  sfml_ui/SFMLApp.cpp sfml_ui/UITheme.cpp sfml_ui/UIButton.cpp sfml_ui/UITextInput.cpp `
  sfml_ui/UISidebar.cpp sfml_ui/LoginView.cpp sfml_ui/RegisterView.cpp sfml_ui/DashboardView.cpp `
  sfml_ui/ProblemBankView.cpp sfml_ui/ContestView.cpp sfml_ui/ResultView.cpp `
  -lsfml-graphics -lsfml-window -lsfml-system
```

Library names may differ depending on your SFML installation.

## 17. Keep It Lightweight

Do not use:

- textures
- images
- gradients
- animations
- complex layouts
- dynamic UI allocation every frame

Use:

- fixed position rectangles
- fixed size buttons
- one font
- one active screen
- reusable button/input objects

This keeps the SFML implementation simple and easier to manage for OOP and memory requirements.
