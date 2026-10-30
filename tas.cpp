
#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <random>
#include <iomanip>
#include <algorithm>
#include <string>

using namespace std;
using namespace chrono;

volatile long long resultatGlobal = 0;

// =====================================================
//              IMPLEMENTATION FROM SCRATCH
// =====================================================

class TasMax {
private:
    vector<int> tas;

public:

    // Remonter un element
    void monter(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;

            if (tas[i] > tas[parent]) {
                swap(tas[i], tas[parent]);
                i = parent;
            }
            else {
                break;
            }
        }
    }

    // Ajuster le tas vers le bas
    void ajuster(int i = 0) {
        int n = tas.size();

        while (true) {
            int gauche = 2 * i + 1;
            int droite = 2 * i + 2;
            int plusGrand = i;

            if (gauche < n && tas[gauche] > tas[plusGrand])
                plusGrand = gauche;

            if (droite < n && tas[droite] > tas[plusGrand])
                plusGrand = droite;

            if (plusGrand != i) {
                swap(tas[i], tas[plusGrand]);
                i = plusGrand;
            }
            else {
                break;
            }
        }
    }

    // Insertion
    void inserer(int valeur) {
        tas.push_back(valeur);
        monter(tas.size() - 1);
    }

    // Consulter le maximum
    int consulterMax() const {
        return tas[0];
    }

    // Suppression seule : sans ajustement
    int supprimerRacine() {
        int maximum = tas[0];

        tas[0] = tas.back();
        tas.pop_back();

        return maximum;
    }

    // Extraction complete : suppression + ajustement
    int extraireMax() {
        int maximum = supprimerRacine();

        if (!tas.empty())
            ajuster();

        return maximum;
    }

    bool vide() const {
        return tas.empty();
    }
};

// =====================================================
//              GENERATION DES DONNEES
// =====================================================

vector<int> genererElements(int N) {
    vector<int> elements;
    elements.reserve(N);

    mt19937 generateur(42);
    uniform_int_distribution<int> distribution(1, 10000000);

    for (int i = 0; i < N; i++) {
        elements.push_back(distribution(generateur));
    }

    return elements;
}

// Construire un tas Scratch
TasMax construireScratch(const vector<int>& elements) {
    TasMax tas;

    for (int valeur : elements) {
        tas.inserer(valeur);
    }

    return tas;
}

// =====================================================
//              STRUCTURE DES RESULTATS
// =====================================================

struct Resultats {
    double insertion;
    double consultation;
    double suppression;
    double ajustement;
    double extractionComplete;
};

// =====================================================
//              TEST FROM SCRATCH
// =====================================================

Resultats testerScratch(const vector<int>& elements) {

    Resultats res = {0, 0, 0, 0, 0};

    TasMax tas;

    // 1. INSERTION
    auto debut = steady_clock::now();

    for (int valeur : elements) {
        tas.inserer(valeur);
    }

    auto fin = steady_clock::now();

    res.insertion =
        duration<double, nano>(fin - debut).count();


    // 2. CONSULTATION DU MAXIMUM
    debut = steady_clock::now();

    int maximum = tas.consulterMax();

    fin = steady_clock::now();

    res.consultation =
        duration<double, nano>(fin - debut).count();

    resultatGlobal += maximum;


    // 3. SUPPRESSION SEULE
    // Reconstruction hors mesure
    tas = construireScratch(elements);

    debut = steady_clock::now();

    int supprime = tas.supprimerRacine();

    fin = steady_clock::now();

    res.suppression =
        duration<double, nano>(fin - debut).count();

    resultatGlobal += supprime;


    // 4. AJUSTEMENT SEUL
    // Construction et suppression hors mesure
    tas = construireScratch(elements);
    tas.supprimerRacine();

    debut = steady_clock::now();

    if (!tas.vide()) {
        tas.ajuster();
    }

    fin = steady_clock::now();

    res.ajustement =
        duration<double, nano>(fin - debut).count();

    if (!tas.vide()) {
        resultatGlobal += tas.consulterMax();
    }


    // 5. EXTRACTION COMPLETE
    // Reconstruction hors mesure
    tas = construireScratch(elements);

    long long somme = 0;

    debut = steady_clock::now();

    while (!tas.vide()) {
        somme += tas.extraireMax();
    }

    fin = steady_clock::now();

    res.extractionComplete =
        duration<double, nano>(fin - debut).count();

    resultatGlobal += somme;

    return res;
}

// =====================================================
//              TEST STL
// =====================================================

Resultats testerSTL(const vector<int>& elements) {

    Resultats res = {0, 0, 0, 0, 0};

    priority_queue<int> tas;

    // 1. INSERTION
    auto debut = steady_clock::now();

    for (int valeur : elements) {
        tas.push(valeur);
    }

    auto fin = steady_clock::now();

    res.insertion =
        duration<double, nano>(fin - debut).count();


    // 2. CONSULTATION DU MAXIMUM
    debut = steady_clock::now();

    int maximum = tas.top();

    fin = steady_clock::now();

    res.consultation =
        duration<double, nano>(fin - debut).count();

    resultatGlobal += maximum;


    // 3 et 4. SUPPRESSION SEULE ET AJUSTEMENT SEUL
    // Non disponibles separement avec priority_queue.


    // 5. EXTRACTION COMPLETE
    long long somme = 0;

    debut = steady_clock::now();

    while (!tas.empty()) {
        somme += tas.top();
        tas.pop();
    }

    fin = steady_clock::now();

    res.extractionComplete =
        duration<double, nano>(fin - debut).count();

    resultatGlobal += somme;

    return res;
}

// =====================================================
//              AFFICHAGE DU TABLEAU
// =====================================================

void afficherEnteteTableau() {

    cout << "\n";
    cout << "===============================================================================================\n";
    cout << "                              COMPARAISON DES TAS MAXIMUM\n";
    cout << "                                      TEMPS EN ns\n";
    cout << "===============================================================================================\n";

    cout << left << setw(25) << "OPERATION"
         << right
         << "|" << setw(13) << "Scratch"
         << setw(13) << "STL"
         << " |" << setw(13) << "Scratch"
         << setw(13) << "STL"
         << " |" << setw(13) << "Scratch"
         << setw(13) << "STL" << endl;

    cout << left << setw(25) << ""
         << right
         << "|" << setw(13) << "N=100"
         << setw(13) << "N=100"
         << " |" << setw(13) << "N=10000"
         << setw(13) << "N=10000"
         << " |" << setw(13) << "N=1000000"
         << setw(13) << "N=1000000" << endl;

    cout << "-----------------------------------------------------------------------------------------------\n";
}

void afficherLigne(
    string operation,
    double s100, double stl100,
    double s10000, double stl10000,
    double s1000000, double stl1000000
) {

    cout << left << setw(25) << operation
         << right << fixed << setprecision(2)
         << "|" << setw(13) << s100
         << setw(13) << stl100
         << " |" << setw(13) << s10000
         << setw(13) << stl10000
         << " |" << setw(13) << s1000000
         << setw(13) << stl1000000
         << endl;
}

// Afficher une ligne avec N/A pour les operations non disponibles en STL
void afficherLigneNA(
    string operation,
    double s100,
    double s10000,
    double s1000000
) {

    cout << left << setw(25) << operation
         << right << fixed << setprecision(2)
         << "|" << setw(13) << s100
         << setw(13) << "N/A"
         << " |" << setw(13) << s10000
         << setw(13) << "N/A"
         << " |" << setw(13) << s1000000
         << setw(13) << "N/A"
         << endl;
}

// =====================================================
//              COMPARAISON DES TAILLES
// =====================================================

void comparerToutesLesTailles() {

    // Generation des donnees
    vector<int> elements100 = genererElements(100);
    vector<int> elements10000 = genererElements(10000);
    vector<int> elements1000000 = genererElements(1000000);

    // Tests Scratch
    Resultats scratch100 = testerScratch(elements100);
    Resultats scratch10000 = testerScratch(elements10000);
    Resultats scratch1000000 = testerScratch(elements1000000);

    // Tests STL
    Resultats stl100 = testerSTL(elements100);
    Resultats stl10000 = testerSTL(elements10000);
    Resultats stl1000000 = testerSTL(elements1000000);

    // Affichage
    afficherEnteteTableau();

    afficherLigne(
        "Insertion",
        scratch100.insertion, stl100.insertion,
        scratch10000.insertion, stl10000.insertion,
        scratch1000000.insertion, stl1000000.insertion
    );

    afficherLigne(
        "Consultation max",
        scratch100.consultation, stl100.consultation,
        scratch10000.consultation, stl10000.consultation,
        scratch1000000.consultation, stl1000000.consultation
    );

    afficherLigneNA(
        "Suppression seule",
        scratch100.suppression,
        scratch10000.suppression,
        scratch1000000.suppression
    );

    afficherLigneNA(
        "Ajustement seul",
        scratch100.ajustement,
        scratch10000.ajustement,
        scratch1000000.ajustement
    );

    afficherLigne(
        "Extraction complete",
        scratch100.extractionComplete, stl100.extractionComplete,
        scratch10000.extractionComplete, stl10000.extractionComplete,
        scratch1000000.extractionComplete, stl1000000.extractionComplete
    );

    cout << "-----------------------------------------------------------------------------------------------\n";

    cout << "\nN/A : operation non mesurable separement avec priority_queue.\n";
}

// =====================================================
//                      MAIN
// =====================================================

int main() {

    cout << "\n";
    cout << "###############################################################\n";
    cout << "#          ETUDE EXPERIMENTALE DU TAS MAXIMUM                 #\n";
    cout << "#                 FROM SCRATCH VS STL                         #\n";
    cout << "###############################################################\n";

    comparerToutesLesTailles();

    cout << "\n";
    cout << "===============================================================\n";
    cout << "                    FIN DES TESTS\n";
    cout << "===============================================================\n";

    return 0;
}
