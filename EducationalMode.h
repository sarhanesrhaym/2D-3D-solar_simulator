#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <functional>
#include <ctime>
#include <cstdlib>

/**
 * @brief Question de quiz avec 4 choix possibles
 */
struct QuizQuestion {
    std::string question;
  std::vector<std::string> choices;  // 4 choix
    int correctAnswer;  // Index de la bonne réponse (0-3)
    std::string explanation;  // Explication après la réponse
    
    QuizQuestion(const std::string& q, const std::vector<std::string>& ch, 
   int correct, const std::string& expl)
        : question(q), choices(ch), correctAnswer(correct), explanation(expl) {}
};

/**
 * @brief Carte pédagogique avec un fait astronomique
 */
struct LearningCard {
    std::string title;
    std::string fact;
    std::string category;  // "Planète", "Mission", "Physique"
    sf::Color accentColor;
    
    LearningCard(const std::string& t, const std::string& f, 
       const std::string& cat, sf::Color col)
        : title(t), fact(f), category(cat), accentColor(col) {}
};

/**
 * @brief Gestionnaire du mode éducatif
 */
class EducationalMode {
public:
    EducationalMode(const sf::Font& font);
    
    // === QUIZ ===
    void startQuiz();
    void nextQuestion();
    void answerQuestion(int choiceIndex);
    void drawQuiz(sf::RenderWindow& window);
    bool handleQuizClick(const sf::Vector2f& mousePos);
    
    // === CARTES PÉDAGOGIQUES ===
    void showNextCard();
    void drawLearningCard(sf::RenderWindow& window, float time);
    
    // === ÉTAT ===
    bool isQuizActive() const { return quizActive; }
    bool isCardActive() const { return cardActive; }
    void setQuizActive(bool active) { quizActive = active; }
    void setCardActive(bool active) { cardActive = active; }
    
    // === STATISTIQUES ===
    int getScore() const { return score; }
    int getTotalQuestions() const { return totalAnswered; }
    float getAccuracy() const;

    //  Mise à jour du bouton
    void updateReturnButton(const sf::Vector2f& mousePos);
    bool isReturnButtonClicked(const sf::Vector2f& mousePos);
    
private:
    // === DONNÉES ===
    std::vector<QuizQuestion> quizQuestions;
    std::vector<LearningCard> learningCards;
    
    // === ÉTAT QUIZ ===
    bool quizActive;
    int currentQuestionIndex;
    int score;
    int totalAnswered;
    bool answerRevealed;
    int selectedAnswer;
    float feedbackTimer;
    
    // === ÉTAT CARTES ===
    bool cardActive;
    int currentCardIndex;
    float cardTimer;

  
    
// === UI ===
    const sf::Font& font;

    //  Bouton de retour
    sf::RectangleShape returnButton;
    sf::Text returnButtonText;
    bool returnButtonHovered;
    
    // === MÉTHODES PRIVÉES ===
    void initializeQuestions();
    void initializeLearningCards();
    void drawQuizQuestion(sf::RenderWindow& window);
    void drawQuizChoices(sf::RenderWindow& window);
 void drawQuizFeedback(sf::RenderWindow& window);
    void drawQuizStats(sf::RenderWindow& window);
};
