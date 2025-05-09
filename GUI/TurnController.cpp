#include "TurnController.hpp"

TurnController::TurnController(Game& game, sf::Font& font, sf::RenderWindow& wnd)
            : game(game), font(font), wnd(wnd)
{
    // prepare action buttons
    float x = 20.f, y = 20.f;
    for (const auto& name : game.getActionNames()) {
        sf::RectangleShape b({120.f, 40.f});
        b.setPosition(x, y);
        b.setFillColor({100,100,200});
        actions.push_back(name);
        btns.push_back(b);
        sf::Text t(name, font, 18);
        t.setFillColor(sf::Color::White);
        centerTextIn(b, t);
        btnLabels.push_back(t);
        x += 140.f;
    }
    // current-player label
    currentPlayer.setFont(font);
    currentPlayer.setCharacterSize(20);
    currentPlayer.setFillColor(sf::Color::White);

    // coin label
    coinLabel.setFont(font);
    coinLabel.setCharacterSize(18);
    coinLabel.setFillColor(sf::Color::Yellow);
    coinLabel.setPosition(20.f, wnd.getSize().y - 40.f);
    // Role label
    roleLabel.setFont(font);
    roleLabel.setCharacterSize(32);
    roleLabel.setFillColor(sf::Color::White);

    // error label
    errorLabel.setFont(font);
    errorLabel.setCharacterSize(24);
    errorLabel.setFillColor(sf::Color::Red);
    errorLabel.setString("");
    centerOrigin(errorLabel);
    errorLabel.setPosition(wnd.getSize().x/2.f, wnd.getSize().y/2.f);

    phase = Phase::StartTurn;
}

void TurnController::handleClick(const sf::Event& evt) {
    if (evt.type != sf::Event::MouseButtonPressed || evt.mouseButton.button != sf::Mouse::Left)
        return;
    sf::Vector2f m(evt.mouseButton.x, evt.mouseButton.y);
    switch (phase) {
        case Phase::ChooseAction:
            for (size_t i=0; i<btns.size(); ++i) {
                if (btns[i].getGlobalBounds().contains(m)) {
                    pendingAction = actions[i];
                    if (game.actionNeedsTarget(pendingAction)) {
                        enterTargetMode();
                        phase = Phase::ChooseTarget;
                    } else {
                        phase = Phase::ResolveAction;
                    }
                    return;
                }
            }
            break;

        case Phase::ChooseTarget:
            for (size_t t=0; t<targetBtns.size(); ++t) {
                if (targetBtns[t].getGlobalBounds().contains(m)) {
                    pendingTarget = otherPlayers[t];
                    phase = Phase::ResolveAction;
                    return;
                }
            }
            break;

        default:
            break;
    }
}

void TurnController::update() {
    switch (phase) {
        case Phase::StartTurn:
            setupForCurrentPlayer();
            phase = Phase::ChooseAction;
            break;

        case Phase::ResolveAction:
            applyPending();
            phase = Phase::EndTurn;
            break;

        case Phase::EndTurn:
            game.nextTurn();
            phase = Phase::StartTurn;
            break;

        default:
            break;
    }
}

void TurnController::render() {
    // action row
    for (auto& b : btns)      wnd.draw(b);
    for (auto& l : btnLabels) wnd.draw(l);
    // target row if needed
    if (phase == Phase::ChooseTarget) {
        for (auto& b : targetBtns) wnd.draw(b);
        for (auto& l : targetLbls) wnd.draw(l);
    }
    // status texts
    wnd.draw(currentPlayer);
    wnd.draw(coinLabel);
    wnd.draw(roleLabel);
    if (!errorLabel.getString().isEmpty())
        wnd.draw(errorLabel);
}

// helpers
void TurnController::setupForCurrentPlayer() {
    clearUI();
    // update player name & coins
    auto* p = game.getCurrentTurn();
    currentPlayer.setString(p->getName() + "'s Turn");
    roleLabel.setString("Role: " + game.getCurrentTurn()->getClassName());
    coinLabel.setString("Coins: " + std::to_string(p->getCoins()));
    errorLabel.setString("");

    auto tb = currentPlayer.getLocalBounds();
    currentPlayer.setOrigin(tb.left + tb.width, tb.top + tb.height);
    float marginX = 20.f, marginY = 20.f;
    currentPlayer.setPosition(wnd.getSize().x - marginX, wnd.getSize().y - marginY);

    auto rl = roleLabel.getLocalBounds();
    roleLabel.setOrigin(rl.left + rl.width/2.f, rl.top  + rl.height);
    float marginY2 = 60.f;
    roleLabel.setPosition(wnd.getSize().x/2.f, wnd.getSize().y - marginY2);
}

void TurnController::applyPending() {
    auto* actor = game.getCurrentTurn();
    if (game.canPerform(actor, pendingAction, pendingTarget)) {
        game.perform(actor, pendingAction, pendingTarget);
    } else {
        errorLabel.setString("Illegal move!");
        centerOrigin(errorLabel);
        phase = Phase::ChooseAction;
        return;
    }
}

void TurnController::enterTargetMode()
{
    targetBtns.clear(); targetLbls.clear(); otherPlayers.clear();
    float x = 20.f, y = 80.f + 60.f;
    for (auto* p : game.getPlayers()) {
        if (p == game.getCurrentTurn()) continue;
        otherPlayers.push_back(p);
        sf::RectangleShape b({120.f,40.f}); b.setPosition(x,y); b.setFillColor({200,80,80});
        targetBtns.push_back(b);
        sf::Text t(p->getName(), font, 18);
        t.setFillColor(sf::Color::White);
        centerTextIn(b, t);
        targetLbls.push_back(t);
        x += 140.f;
    }
}

void TurnController::clearUI()
{
    targetBtns.clear(); targetLbls.clear(); otherPlayers.clear();
}

void TurnController::centerOrigin(sf::Text& t)
{
    auto b = t.getLocalBounds();
    t.setOrigin(b.left + b.width/2.f, b.top + b.height/2.f);
}

void TurnController::centerTextIn(const sf::RectangleShape& b, sf::Text& t)
{
    auto bb = b.getGlobalBounds();
    auto tb = t.getLocalBounds();
    t.setPosition(
            bb.left + (bb.width - tb.width)/2 - tb.left,
            bb.top  + (bb.height - tb.height)/2 - tb.top
    );
}