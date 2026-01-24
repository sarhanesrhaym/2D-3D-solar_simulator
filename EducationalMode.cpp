#include "EducationalMode.h"
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <cmath>

EducationalMode::EducationalMode(const sf::Font& f)
    : font(f), quizActive(false), currentQuestionIndex(0), score(0),
    totalAnswered(0), answerRevealed(false), selectedAnswer(-1),
    feedbackTimer(0.f), cardActive(false), currentCardIndex(0), cardTimer(0.f) {
    initializeQuestions();
    initializeLearningCards();
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    // Initialiser le bouton de retour
    returnButton.setSize(sf::Vector2f(140.f, 45.f));
    returnButton.setPosition(20.f, 20.f);
    returnButton.setFillColor(sf::Color(100, 60, 140, 220));
    returnButton.setOutlineThickness(2.f);
    returnButton.setOutlineColor(sf::Color(180, 150, 255));

    returnButtonText.setFont(font);
    returnButtonText.setString("RETOUR");
    returnButtonText.setCharacterSize(16);
    returnButtonText.setFillColor(sf::Color::White);
    returnButtonText.setStyle(sf::Text::Bold);
    returnButtonText.setPosition(50.f, 32.f);
}

// ===== INITIALISATION DES QUESTIONS =====
void EducationalMode::initializeQuestions() {
    quizQuestions.clear();

    quizQuestions.push_back(QuizQuestion(
        "Quelle est la particularite de Mercure ?",
        { "C'est la plus grosse planete", "Sa journee dure plus longtemps que son annee",
         "Elle possede des anneaux", "Elle est la plus froide" },
        1,
        "Mercure tourne si lentement qu'une journee (176 jours terrestres) dure plus longtemps que son annee (88 jours) !"
    ));

    quizQuestions.push_back(QuizQuestion(
        "Combien de lunes possede Mercure ?",
        { "0", "1", "2", "7" },
        0,
        "Mercure est trop proche du Soleil pour retenir des satellites naturels."
    ));

    quizQuestions.push_back(QuizQuestion(
        "Quelle est la temperature moyenne sur Venus ?",
        { "100 C", "250 C", "464 C", "1000 C" },
        2,
        "Venus est la planete la plus chaude avec 464 C en raison de son effet de serre extreme !"
    ));

    quizQuestions.push_back(QuizQuestion(
        "Dans quel sens tourne Venus sur elle-meme ?",
        { "Sens normal (prograde)", "Sens inverse (retrograde)", "Elle ne tourne pas", "Variable" },
        1,
        "Venus tourne en sens retrograde, probablement suite a une collision geante."
    ));

    quizQuestions.push_back(QuizQuestion(
        "Quel pourcentage de la surface terrestre est recouverte d'eau ?",
        { "50%", "61%", "71%", "85%" },
        2,
        "71% de la surface terrestre est recouverte d'oceans et de mers."
    ));

    quizQuestions.push_back(QuizQuestion(
        "Quel est l'age approximatif de la Terre ?",
        { "2 milliards d'annees", "3,5 milliards d'annees",
         "4,5 milliards d'annees", "6 milliards d'annees" },
        2,
        "La Terre s'est formee il y a environ 4,5 milliards d'annees."
    ));

    quizQuestions.push_back(QuizQuestion(
        "Quel est le plus grand volcan du systeme solaire ?",
        { "Mont Everest", "Olympus Mons sur Mars", "Mauna Kea", "Volcan Io" },
        1,
        "Olympus Mons sur Mars culmine a 21 km, soit 3 fois la hauteur de l'Everest !"
    ));

    quizQuestions.push_back(QuizQuestion(
        "Combien de rovers ont ete envoyes avec succes sur Mars par la NASA ?",
        { "2", "3", "5", "7" },
        2,
        "5 rovers : Sojourner (1997), Spirit (2004), Opportunity (2004), Curiosity (2012), Perseverance (2021)."
    ));

    quizQuestions.push_back(QuizQuestion(
        "Combien de Terres pourraient tenir dans Jupiter ?",
        { "100", "500", "1 300", "10 000" },
        2,
        "Jupiter pourrait contenir plus de 1 300 Terres ! C'est la plus grande planete."
    ));

    quizQuestions.push_back(QuizQuestion(
        "Quel phenomene celebre peut-on observer sur Jupiter ?",
        { "Anneaux geants", "Grande Tache Rouge", "Geysers de glace", "Aurores polaires" },
        1,
        "La Grande Tache Rouge est une tempete geante qui dure depuis au moins 400 ans !"
    ));

    quizQuestions.push_back(QuizQuestion(
        "De quoi sont composes les anneaux de Saturne ?",
        { "Roches", "Glace et roches", "Poussiere", "Gaz" },
        1,
        "Les anneaux sont composes de milliards de particules de glace et de roches."
    ));

    quizQuestions.push_back(QuizQuestion(
        "Saturne flotterait-elle dans l'eau ?",
        { "Oui", "Non", "Partiellement", "Seulement a haute temperature" },
        0,
        "Saturne a une densite inferieure a l'eau (0,687 g/cm3), elle flotterait !"
    ));

    quizQuestions.push_back(QuizQuestion(
        "Quelle est l'inclinaison de l'axe de rotation d'Uranus ?",
        { "23 degres", "45 degres", "82 degres", "98 degres" },
        3,
        "Uranus tourne quasiment sur le cote avec une inclinaison de 98 degres !"
    ));

    quizQuestions.push_back(QuizQuestion(
        "Quelle est la vitesse maximale des vents sur Neptune ?",
        { "500 km/h", "1 000 km/h", "2 100 km/h", "3 500 km/h" },
        2,
        "Neptune possede les vents les plus rapides du systeme solaire a 2 100 km/h !"
    ));

    quizQuestions.push_back(QuizQuestion(
        "Quelle sonde a survole Pluton en 2015 ?",
        { "Voyager 1", "New Horizons", "Pioneer 10", "Cassini" },
        1,
        "New Horizons a survole Pluton le 14 juillet 2015 apres 9 ans de voyage."
    ));

    quizQuestions.push_back(QuizQuestion(
        "Quelle mission a decouvert des geysers sur Encelade ?",
        { "Voyager 2", "Galileo", "Cassini", "Juno" },
        2,
        "Cassini a decouvert des geysers d'eau sur Encelade, lune de Saturne."
    ));
}

// ===== INITIALISATION DES CARTES PÉDAGOGIQUES =====
void EducationalMode::initializeLearningCards() {
    learningCards.clear();

    learningCards.push_back(LearningCard(
        "Mercure : La Planete Extreme",
        "Sur Mercure, la difference de temperature entre le jour (+430 C) et la nuit (-180 C) est de 610 C ! "
        "C'est l'amplitude thermique la plus importante du systeme solaire.",
        "Planete", sf::Color(169, 169, 169)
    ));

    learningCards.push_back(LearningCard(
        "Venus : L'Enfer Celeste",
        "Venus possede une atmosphere si dense que la pression au sol equivaut a etre a 900 metres sous l'eau ! "
        "Les sondes sovietiques Venera n'ont survecu que quelques heures.",
        "Planete", sf::Color(255, 198, 73)
    ));

    learningCards.push_back(LearningCard(
        "Terre : Notre Oasis Cosmique",
        "La Terre est la seule planete connue ou l'eau existe sous ses trois formes : solide (glace), "
        "liquide (oceans) et gazeuse (vapeur d'eau). Cette caracteristique unique permet la vie.",
        "Planete", sf::Color(100, 149, 237)
    ));

    learningCards.push_back(LearningCard(
        "Mars : La Planete Rouge",
        "Mars possede Valles Marineris, un canyon long de 4 000 km et profond de 7 km. "
        "Il pourrait contenir tout le territoire des Etats-Unis !",
        "Planete", sf::Color(205, 92, 92)
    ));

    learningCards.push_back(LearningCard(
        "Jupiter : Le Geant Protecteur",
        "Jupiter agit comme un bouclier pour la Terre en deviant des asteroides et cometes dangereux "
        "grace a sa gravite immense. Sans Jupiter, la vie sur Terre serait menacee !",
        "Planete", sf::Color(218, 165, 32)
    ));

    learningCards.push_back(LearningCard(
        "Saturne : La Planete aux Mille Anneaux",
        "Les anneaux de Saturne s'etendent sur 282 000 km mais ne font que 10 metres d'epaisseur ! "
        "C'est comme une feuille de papier de la taille d'un terrain de football.",
        "Planete", sf::Color(238, 232, 170)
    ));

    learningCards.push_back(LearningCard(
        "Voyager 1 : Le Messager de l'Humanite",
        "Voyager 1 (1977) est l'objet le plus eloigne cree par l'homme (24 milliards de km). "
        "Il transporte un disque d'or avec des sons et images de la Terre pour d'eventuelles civilisations.",
        "Mission", sf::Color(100, 180, 255)
    ));

    learningCards.push_back(LearningCard(
        "Apollo 11 : Un Petit Pas...",
        "Le 21 juillet 1969, Neil Armstrong devient le premier humain a marcher sur la Lune. "
        "Ses empreintes sont toujours visibles et le resteront pendant des millions d'annees !",
        "Mission", sf::Color(200, 200, 200)
    ));

    learningCards.push_back(LearningCard(
        "La Gravite : La Force Invisible",
        "La gravite de Jupiter est 2,5 fois plus forte que celle de la Terre. "
        "Un humain de 70 kg peserait 175 kg sur Jupiter !",
        "Physique", sf::Color(255, 150, 50)
    ));

    learningCards.push_back(LearningCard(
        "Vitesse de la Lumiere",
        "La lumiere du Soleil met 8 minutes et 20 secondes pour atteindre la Terre. "
        "Quand vous regardez le Soleil, vous le voyez tel qu'il etait il y a 8 minutes !",
        "Physique", sf::Color(255, 255, 100)
    ));
}

// ===== QUIZ : DÉMARRAGE =====
void EducationalMode::startQuiz() {
    std::cout << "DEBUG: Quiz demarre !" << std::endl;
    quizActive = true;
    currentQuestionIndex = 0;
    score = 0;
    totalAnswered = 0;
    answerRevealed = false;
    selectedAnswer = -1;

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(quizQuestions.begin(), quizQuestions.end(), g);
    std::cout << "DEBUG: " << quizQuestions.size() << " questions chargees" << std::endl;
}

// ===== QUIZ : QUESTION SUIVANTE =====
void EducationalMode::nextQuestion() {
    if (currentQuestionIndex < static_cast<int>(quizQuestions.size()) - 1) {
        currentQuestionIndex++;
        answerRevealed = false;
        selectedAnswer = -1;
        feedbackTimer = 0.f;
    }
    else {
        quizActive = false;
    }
}

// ===== QUIZ : RÉPONDRE =====
void EducationalMode::answerQuestion(int choiceIndex) {
    if (answerRevealed) return;

    selectedAnswer = choiceIndex;
    answerRevealed = true;
    totalAnswered++;

    if (choiceIndex == quizQuestions[currentQuestionIndex].correctAnswer) {
        score++;
    }

    feedbackTimer = 0.f;
}

// ===== QUIZ : GESTION DES CLICS =====
bool EducationalMode::handleQuizClick(const sf::Vector2f& mousePos) {
    if (!quizActive) return false;

    if (isReturnButtonClicked(mousePos)) {
        return true;
    }

    if (answerRevealed) {
        sf::FloatRect nextBtnBounds(500.f, 660.f, 200.f, 55.f);
        if (nextBtnBounds.contains(mousePos)) {
            nextQuestion();
            return true;
        }
        return false;
    }

    float choiceY = 280.f;
    float choiceH = 65.f;
    float spacing = 12.f;

    for (int i = 0; i < 4; ++i) {
        sf::FloatRect choiceBounds(200.f, choiceY + i * (choiceH + spacing), 800.f, choiceH);
        if (choiceBounds.contains(mousePos)) {
            answerQuestion(i);
            return true;
        }
    }

    return false;
}

// ===== QUIZ : DESSIN PRINCIPAL =====
void EducationalMode::drawQuiz(sf::RenderWindow& window) {
    if (!quizActive || currentQuestionIndex >= static_cast<int>(quizQuestions.size())) return;

    static float animTime = 0.f;
    animTime += 0.016f;

    // ===== FOND AVEC DÉGRADÉ CLAIR =====
    for (int i = 0; i < 8; ++i) {
        float ratio = static_cast<float>(i) / 8.f;
        sf::RectangleShape bgLayer(sf::Vector2f(1200.f, 100.f));
        bgLayer.setPosition(0.f, i * 100.f);

        // Dégradé lavande vers bleu clair
        sf::Uint8 r = static_cast<sf::Uint8>(180 - 40 * ratio);
        sf::Uint8 g = static_cast<sf::Uint8>(200 - 30 * ratio);
        sf::Uint8 b = static_cast<sf::Uint8>(240 - 20 * ratio);
        bgLayer.setFillColor(sf::Color(r, g, b, 255));
        window.draw(bgLayer);
    }

    // ===== ÉTOILES DÉCORATIVES =====
    for (int i = 0; i < 25; ++i) {
        float x = static_cast<float>((i * 47) % 1200);
        float y = static_cast<float>((i * 31) % 800);
        float twinkle = 100.f + 80.f * std::sin(animTime * 2.5f + i);

        sf::CircleShape star(1.5f + std::sin(animTime + i) * 0.5f);
        star.setPosition(x, y);
        star.setFillColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(twinkle)));
        window.draw(star);
    }

    // ===== PANNEAU PRINCIPAL =====
    float panelX = 170.f;
    float panelY = 60.f;
    float panelW = 860.f;
    float panelH = 680.f;

    // Ombre
    sf::RectangleShape panelShadow(sf::Vector2f(panelW, panelH));
    panelShadow.setPosition(panelX + 6.f, panelY + 6.f);
    panelShadow.setFillColor(sf::Color(0, 0, 0, 40));
    window.draw(panelShadow);

    // Fond blanc avec transparence
    sf::RectangleShape panel(sf::Vector2f(panelW, panelH));
    panel.setPosition(panelX, panelY);
    panel.setFillColor(sf::Color(255, 255, 255, 245));
    panel.setOutlineThickness(3.f);
    float borderPulse = 180.f + 60.f * std::sin(animTime * 2.f);
    panel.setOutlineColor(sf::Color(120, 80, 200, static_cast<sf::Uint8>(borderPulse)));
    window.draw(panel);

    // ===== EN-TÊTE DÉGRADÉ VIOLET-ROSE =====
    for (int i = 0; i < 4; ++i) {
        float ratio = static_cast<float>(i) / 4.f;
        sf::RectangleShape headerLayer(sf::Vector2f(panelW, 20.f));
        headerLayer.setPosition(panelX, panelY + i * 20.f);

        sf::Uint8 r = static_cast<sf::Uint8>(160 - 20 * ratio);
        sf::Uint8 g = static_cast<sf::Uint8>(100 - 30 * ratio);
        sf::Uint8 b = static_cast<sf::Uint8>(220 - 10 * ratio);
        headerLayer.setFillColor(sf::Color(r, g, b, 255));
        window.draw(headerLayer);
    }

    // Ligne brillante
    sf::RectangleShape headerGlow(sf::Vector2f(panelW, 3.f));
    headerGlow.setPosition(panelX, panelY + 78.f);
    float glowAlpha = 200.f + 55.f * std::sin(animTime * 4.f);
    headerGlow.setFillColor(sf::Color(255, 200, 255, static_cast<sf::Uint8>(glowAlpha)));
    window.draw(headerGlow);

    // Titre
    sf::Text title("QUIZ ASTRONOMIQUE", font, 32);
    title.setPosition(panelX + panelW / 2.f, panelY + 22.f);
    title.setOrigin(title.getLocalBounds().width / 2.f, 0.f);
    title.setFillColor(sf::Color::White);
    title.setStyle(sf::Text::Bold);
    title.setOutlineThickness(2.f);
    title.setOutlineColor(sf::Color(100, 50, 150));
    window.draw(title);

    drawQuizStats(window);
    drawQuizQuestion(window);
    drawQuizChoices(window);

    if (answerRevealed) {
        drawQuizFeedback(window);
    }

    // ===== BOUTON RETOUR MODERNE =====
    for (int i = 0; i < 2; ++i) {
        float ratio = static_cast<float>(i) / 2.f;
        sf::RectangleShape retLayer(sf::Vector2f(140.f, 22.f));
        retLayer.setPosition(20.f, 20.f + i * 22.f);

        if (returnButtonHovered) {
            retLayer.setFillColor(sf::Color(
                static_cast<sf::Uint8>(140 - 20 * ratio),
                static_cast<sf::Uint8>(100 - 20 * ratio),
                static_cast<sf::Uint8>(200 - 30 * ratio), 240));
        }
        else {
            retLayer.setFillColor(sf::Color(
                static_cast<sf::Uint8>(120 - 20 * ratio),
                static_cast<sf::Uint8>(80 - 20 * ratio),
                static_cast<sf::Uint8>(180 - 30 * ratio), 220));
        }
        window.draw(retLayer);
    }

    sf::RectangleShape retBorder(sf::Vector2f(140.f, 44.f));
    retBorder.setPosition(20.f, 20.f);
    retBorder.setFillColor(sf::Color::Transparent);
    retBorder.setOutlineThickness(2.f);
    retBorder.setOutlineColor(returnButtonHovered ? sf::Color(220, 180, 255) : sf::Color(180, 140, 220));
    window.draw(retBorder);

    sf::Text retText("RETOUR", font, 15);
    retText.setPosition(90.f, 32.f);
    retText.setOrigin(retText.getLocalBounds().width / 2.f, 0.f);
    retText.setFillColor(sf::Color::White);
    retText.setStyle(sf::Text::Bold);
    window.draw(retText);

    returnButton.setPosition(20.f, 20.f);
    returnButton.setSize(sf::Vector2f(140.f, 44.f));
}

// ===== QUIZ : QUESTION =====
void EducationalMode::drawQuizQuestion(sf::RenderWindow& window) {
    const auto& q = quizQuestions[currentQuestionIndex];

    float panelX = 170.f;
    float panelW = 860.f;

    // Badge numéro de question
    sf::RectangleShape questionBadge(sf::Vector2f(150.f, 32.f));
    questionBadge.setPosition(panelX + 20.f, 100.f);
    questionBadge.setFillColor(sf::Color(100, 180, 220, 230));
    questionBadge.setOutlineThickness(2.f);
    questionBadge.setOutlineColor(sf::Color(150, 210, 255));
    window.draw(questionBadge);

    std::ostringstream oss;
    oss << "Question " << (currentQuestionIndex + 1) << " / " << quizQuestions.size();
    sf::Text questionNum(oss.str(), font, 14);
    questionNum.setPosition(panelX + 30.f, 106.f);
    questionNum.setFillColor(sf::Color::White);
    questionNum.setStyle(sf::Text::Bold);
    window.draw(questionNum);

    // Fond de la question
    sf::RectangleShape questionBg(sf::Vector2f(panelW - 40.f, 70.f));
    questionBg.setPosition(panelX + 20.f, 145.f);
    questionBg.setFillColor(sf::Color(250, 248, 255, 255));
    questionBg.setOutlineThickness(2.f);
    questionBg.setOutlineColor(sf::Color(200, 180, 230));
    window.draw(questionBg);

    // Icône question
    sf::CircleShape questionIcon(16.f);
    questionIcon.setPosition(panelX + 35.f, 162.f);
    questionIcon.setFillColor(sf::Color(255, 180, 100));
    questionIcon.setOutlineThickness(2.f);
    questionIcon.setOutlineColor(sf::Color::White);
    window.draw(questionIcon);

    sf::Text qMark("?", font, 20);
    qMark.setPosition(panelX + 46.f, 164.f);
    qMark.setOrigin(qMark.getLocalBounds().width / 2.f, 0.f);
    qMark.setFillColor(sf::Color::White);
    qMark.setStyle(sf::Text::Bold);
    window.draw(qMark);

    // Texte question
    sf::Text questionText(q.question, font, 18);
    questionText.setPosition(panelX + 80.f, 168.f);
    questionText.setFillColor(sf::Color(60, 50, 80));
    questionText.setStyle(sf::Text::Bold);
    window.draw(questionText);
}

// ===== QUIZ : CHOIX  =====
void EducationalMode::drawQuizChoices(sf::RenderWindow& window) {
    const auto& q = quizQuestions[currentQuestionIndex];

    static float animTime = 0.f;
    animTime += 0.016f;

    float panelX = 170.f;
    float panelW = 860.f;
    float choiceY = 235.f;
    float choiceH = 58.f;
    float choiceW = panelW - 60.f;
    float spacing = 10.f;

    // Couleurs pastel modernes pour chaque choix
    std::vector<sf::Color> pastelColors = {
        sf::Color(255, 180, 180),  // Rose pastel
        sf::Color(180, 220, 255),  // Bleu pastel
        sf::Color(180, 255, 200),  // Vert pastel
        sf::Color(255, 220, 160)   // Orange pastel
    };

    std::vector<sf::Color> accentColors = {
        sf::Color(220, 100, 120),  // Rose accent
        sf::Color(80, 150, 220),   // Bleu accent
        sf::Color(80, 200, 120),   // Vert accent
        sf::Color(240, 160, 60)    // Orange accent
    };

    char letters[] = { 'A', 'B', 'C', 'D' };

    for (int i = 0; i < 4; ++i) {
        float currentY = choiceY + i * (choiceH + spacing);

        bool isCorrect = (i == q.correctAnswer);
        bool isSelected = (i == selectedAnswer);

        sf::Color bgColor, borderColor, letterBgColor;

        if (answerRevealed) {
            if (isCorrect) {
                // Bonne réponse - Vert lumineux
                bgColor = sf::Color(200, 255, 210, 255);
                borderColor = sf::Color(50, 200, 100);
                letterBgColor = sf::Color(50, 180, 90);
            }
            else if (isSelected) {
                // Mauvaise réponse - Rose/rouge doux
                bgColor = sf::Color(255, 210, 210, 255);
                borderColor = sf::Color(220, 100, 100);
                letterBgColor = sf::Color(200, 80, 80);
            }
            else {
                // Non sélectionné - Gris clair
                bgColor = sf::Color(235, 235, 240, 200);
                borderColor = sf::Color(180, 180, 190);
                letterBgColor = sf::Color(160, 160, 170);
            }
        }
        else {
            // État normal - Couleurs pastel
            bgColor = sf::Color(255, 255, 255, 250);
            borderColor = pastelColors[i];
            letterBgColor = accentColors[i];
        }

        // Ombre
        sf::RectangleShape choiceShadow(sf::Vector2f(choiceW, choiceH));
        choiceShadow.setPosition(panelX + 33.f, currentY + 3.f);
        choiceShadow.setFillColor(sf::Color(0, 0, 0, 25));
        window.draw(choiceShadow);

        // Fond du choix
        sf::RectangleShape choiceBg(sf::Vector2f(choiceW, choiceH));
        choiceBg.setPosition(panelX + 30.f, currentY);
        choiceBg.setFillColor(bgColor);
        choiceBg.setOutlineThickness(3.f);
        choiceBg.setOutlineColor(borderColor);
        window.draw(choiceBg);

        // Barre colorée à gauche
        sf::RectangleShape leftBar(sf::Vector2f(6.f, choiceH));
        leftBar.setPosition(panelX + 30.f, currentY);
        leftBar.setFillColor(answerRevealed ? borderColor : accentColors[i]);
        window.draw(leftBar);

        // Cercle avec lettre
        sf::CircleShape letterCircle(20.f);
        letterCircle.setPosition(panelX + 50.f, currentY + 9.f);
        letterCircle.setFillColor(letterBgColor);
        letterCircle.setOutlineThickness(3.f);
        letterCircle.setOutlineColor(sf::Color::White);
        window.draw(letterCircle);

        sf::Text letterText(std::string(1, letters[i]), font, 20);
        letterText.setPosition(panelX + 70.f, currentY + 16.f);
        letterText.setOrigin(letterText.getLocalBounds().width / 2.f, 0.f);
        letterText.setFillColor(sf::Color::White);
        letterText.setStyle(sf::Text::Bold);
        window.draw(letterText);

        // Texte du choix
        sf::Text choiceText(q.choices[i], font, 16);
        choiceText.setPosition(panelX + 110.f, currentY + 18.f);

        if (answerRevealed) {
            choiceText.setFillColor(isCorrect || isSelected ? sf::Color(40, 40, 50) : sf::Color(120, 120, 130));
        }
        else {
            choiceText.setFillColor(sf::Color(50, 50, 70));
        }
        choiceText.setStyle(sf::Text::Bold);
        window.draw(choiceText);

        // Indicateurs visuels après réponse
        if (answerRevealed) {
            if (isCorrect) {
                sf::CircleShape checkBg(14.f);
                checkBg.setPosition(panelX + choiceW - 20.f, currentY + 15.f);
                checkBg.setFillColor(sf::Color(50, 200, 100));
                window.draw(checkBg);

                sf::Text checkMark("OK", font, 11);
                checkMark.setPosition(panelX + choiceW - 6.f, currentY + 20.f);
                checkMark.setOrigin(checkMark.getLocalBounds().width / 2.f, 0.f);
                checkMark.setFillColor(sf::Color::White);
                checkMark.setStyle(sf::Text::Bold);
                window.draw(checkMark);
            }
            else if (isSelected) {
                sf::CircleShape xBg(14.f);
                xBg.setPosition(panelX + choiceW - 20.f, currentY + 15.f);
                xBg.setFillColor(sf::Color(220, 80, 80));
                window.draw(xBg);

                sf::Text xMark("X", font, 14);
                xMark.setPosition(panelX + choiceW - 6.f, currentY + 18.f);
                xMark.setOrigin(xMark.getLocalBounds().width / 2.f, 0.f);
                xMark.setFillColor(sf::Color::White);
                xMark.setStyle(sf::Text::Bold);
                window.draw(xMark);
            }
        }
    }
}

// ===== QUIZ  =====
void EducationalMode::drawQuizFeedback(sf::RenderWindow& window) {
    const auto& q = quizQuestions[currentQuestionIndex];
    bool correct = (selectedAnswer == q.correctAnswer);

    static float animTime = 0.f;
    animTime += 0.016f;

    float panelX = 170.f;
    float panelW = 860.f;
    float feedbackY = 520.f;

    // Fond du feedback
    sf::RectangleShape feedbackBg(sf::Vector2f(panelW - 60.f, 90.f));
    feedbackBg.setPosition(panelX + 30.f, feedbackY);

    if (correct) {
        feedbackBg.setFillColor(sf::Color(220, 255, 230, 250));
        feedbackBg.setOutlineColor(sf::Color(80, 200, 120));
    }
    else {
        feedbackBg.setFillColor(sf::Color(255, 235, 220, 250));
        feedbackBg.setOutlineColor(sf::Color(255, 150, 100));
    }
    feedbackBg.setOutlineThickness(2.f);
    window.draw(feedbackBg);

    // Icône de résultat
    sf::CircleShape resultIcon(18.f);
    resultIcon.setPosition(panelX + 45.f, feedbackY + 12.f);
    resultIcon.setFillColor(correct ? sf::Color(80, 200, 120) : sf::Color(255, 150, 80));
    window.draw(resultIcon);

    sf::Text resultSymbol(correct ? "+" : "!", font, 22);
    resultSymbol.setPosition(panelX + 63.f, feedbackY + 16.f);
    resultSymbol.setOrigin(resultSymbol.getLocalBounds().width / 2.f, 0.f);
    resultSymbol.setFillColor(sf::Color::White);
    resultSymbol.setStyle(sf::Text::Bold);
    window.draw(resultSymbol);

    // Texte du résultat
    sf::Text resultText(correct ? "BRAVO ! Bonne reponse !" : "Pas tout a fait...", font, 18);
    resultText.setPosition(panelX + 95.f, feedbackY + 15.f);
    resultText.setFillColor(correct ? sf::Color(40, 140, 80) : sf::Color(180, 80, 40));
    resultText.setStyle(sf::Text::Bold);
    window.draw(resultText);

    // Explication
    sf::Text explanation(q.explanation, font, 12);
    explanation.setPosition(panelX + 50.f, feedbackY + 50.f);
    explanation.setFillColor(sf::Color(80, 80, 100));
    window.draw(explanation);

    // ===== BOUTON SUIVANT MODERNE =====
    float btnX = panelX + panelW / 2.f - 100.f;
    float btnY = 660.f;

    // Dégradé du bouton
    for (int i = 0; i < 3; ++i) {
        float ratio = static_cast<float>(i) / 3.f;
        sf::RectangleShape btnLayer(sf::Vector2f(200.f, 18.f));
        btnLayer.setPosition(btnX, btnY + i * 18.f);

        sf::Uint8 r = static_cast<sf::Uint8>(100 + 60 * (1.f - ratio));
        sf::Uint8 g = static_cast<sf::Uint8>(180 + 40 * (1.f - ratio));
        sf::Uint8 b = static_cast<sf::Uint8>(220 + 30 * (1.f - ratio));
        btnLayer.setFillColor(sf::Color(r, g, b, 255));
        window.draw(btnLayer);
    }

    sf::RectangleShape nextBtnBorder(sf::Vector2f(200.f, 55.f));
    nextBtnBorder.setPosition(btnX, btnY);
    nextBtnBorder.setFillColor(sf::Color::Transparent);
    nextBtnBorder.setOutlineThickness(3.f);
    float btnPulse = 200.f + 55.f * std::sin(animTime * 4.f);
    nextBtnBorder.setOutlineColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(btnPulse)));
    window.draw(nextBtnBorder);

    sf::Text nextText("SUIVANT", font, 18);
    nextText.setPosition(btnX + 100.f, btnY + 15.f);
    nextText.setOrigin(nextText.getLocalBounds().width / 2.f, 0.f);
    nextText.setFillColor(sf::Color::White);
    nextText.setStyle(sf::Text::Bold);
    nextText.setOutlineThickness(1.f);
    nextText.setOutlineColor(sf::Color(60, 120, 160));
    window.draw(nextText);

    sf::Text arrow(">>", font, 16);
    arrow.setPosition(btnX + 165.f, btnY + 16.f);
    arrow.setFillColor(sf::Color::White);
    window.draw(arrow);
}

// ===== QUIZ : STATISTIQUES =====
void EducationalMode::drawQuizStats(sf::RenderWindow& window) {
    float panelX = 170.f;
    float panelW = 860.f;

    // Badge score
    sf::RectangleShape scoreBadge(sf::Vector2f(130.f, 50.f));
    scoreBadge.setPosition(panelX + panelW - 160.f, 95.f);
    scoreBadge.setFillColor(sf::Color(100, 200, 150, 230));
    scoreBadge.setOutlineThickness(2.f);
    scoreBadge.setOutlineColor(sf::Color(150, 230, 180));
    window.draw(scoreBadge);

    sf::Text scoreLabel("Score", font, 11);
    scoreLabel.setPosition(panelX + panelW - 95.f, 98.f);
    scoreLabel.setOrigin(scoreLabel.getLocalBounds().width / 2.f, 0.f);
    scoreLabel.setFillColor(sf::Color(230, 255, 240));
    window.draw(scoreLabel);

    std::ostringstream oss;
    oss << score << " / " << totalAnswered;
    sf::Text scoreText(oss.str(), font, 18);
    scoreText.setPosition(panelX + panelW - 95.f, 115.f);
    scoreText.setOrigin(scoreText.getLocalBounds().width / 2.f, 0.f);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setStyle(sf::Text::Bold);
    window.draw(scoreText);

    // Précision (si applicable)
    if (totalAnswered > 0) {
        float accuracy = getAccuracy() * 100.f;

        sf::RectangleShape accBadge(sf::Vector2f(80.f, 25.f));
        accBadge.setPosition(panelX + panelW - 250.f, 107.f);
        accBadge.setFillColor(sf::Color(180, 140, 220, 200));
        window.draw(accBadge);

        oss.str("");
        oss << std::fixed << std::setprecision(0) << accuracy << "%";
        sf::Text accText(oss.str(), font, 12);
        accText.setPosition(panelX + panelW - 210.f, 110.f);
        accText.setOrigin(accText.getLocalBounds().width / 2.f, 0.f);
        accText.setFillColor(sf::Color::White);
        accText.setStyle(sf::Text::Bold);
        window.draw(accText);
    }
}

// ===== CARTES : AFFICHER LA SUIVANTE =====
void EducationalMode::showNextCard() {
    currentCardIndex = (currentCardIndex + 1) % learningCards.size();
    cardTimer = 0.f;
}

// ===== CARTES : DESSIN =====
void EducationalMode::drawLearningCard(sf::RenderWindow& window, float time) {
    if (!cardActive || learningCards.empty()) return;

    const auto& card = learningCards[currentCardIndex];

    float pulse = 1.f + 0.05f * std::sin(time * 2.f);

    sf::RectangleShape cardPanel(sf::Vector2f(500.f * pulse, 300.f * pulse));
    cardPanel.setPosition(350.f, 250.f);
    cardPanel.setOrigin(250.f * pulse, 150.f * pulse);
    cardPanel.setFillColor(sf::Color(255, 255, 255, 245));
    cardPanel.setOutlineThickness(4.f);
    cardPanel.setOutlineColor(card.accentColor);
    window.draw(cardPanel);

    sf::RectangleShape topBar(sf::Vector2f(500.f, 50.f));
    topBar.setPosition(100.f, 100.f);
    topBar.setFillColor(card.accentColor);
    window.draw(topBar);

    sf::Text category(card.category, font, 16);
    category.setPosition(120.f, 115.f);
    category.setFillColor(sf::Color::White);
    category.setStyle(sf::Text::Bold);
    window.draw(category);

    sf::Text title(card.title, font, 22);
    title.setPosition(350.f, 170.f);
    title.setOrigin(title.getLocalBounds().width / 2.f, 0.f);
    title.setFillColor(card.accentColor);
    title.setStyle(sf::Text::Bold);
    window.draw(title);

    sf::Text fact(card.fact, font, 14);
    fact.setPosition(120.f, 220.f);
    fact.setFillColor(sf::Color(80, 80, 100));
    window.draw(fact);
}

// ===== PRÉCISION =====
float EducationalMode::getAccuracy() const {
    if (totalAnswered == 0) return 0.f;
    return static_cast<float>(score) / static_cast<float>(totalAnswered);
}

// ===== BOUTON RETOUR  =====
void EducationalMode::updateReturnButton(const sf::Vector2f& mousePos) {
    sf::FloatRect bounds = returnButton.getGlobalBounds();
    returnButtonHovered = bounds.contains(mousePos);
}

// ===== BOUTON RETOUR : DÉTECTION CLIC =====
bool EducationalMode::isReturnButtonClicked(const sf::Vector2f& mousePos) {
    sf::FloatRect bounds(20.f, 20.f, 140.f, 44.f);
    if (bounds.contains(mousePos)) {
        quizActive = false;
        currentQuestionIndex = 0;
        answerRevealed = false;
        selectedAnswer = -1;
        score = 0;
        totalAnswered = 0;
        return true;
    }
    return false;
}