#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <iomanip>
using namespace std;
using namespace chrono;

struct Noeud {
    int valeur;
    Noeud* gauche;
    Noeud* droite;
    Noeud(int v) : valeur(v), gauche(nullptr), droite(nullptr) {}
};

// Insertion iterative : renvoie le nombre de comparaisons
long long inserer(Noeud*& racine, int v) {
    if (racine == nullptr) {
        racine = new Noeud(v);
        return 0;
    }
    Noeud* c = racine;
    long long comp = 0;
    while (true) {
        comp++;
        if (v < c->valeur) {
            if (!c->gauche) { c->gauche = new Noeud(v); break; }
            c = c->gauche;
        } else if (v > c->valeur) {
            if (!c->droite) { c->droite = new Noeud(v); break; }
            c = c->droite;
        } else break; // doublon
    }
    return comp;
}

int hauteur(Noeud* r) {
    if (!r) return 0;
    return 1 + max(hauteur(r->gauche), hauteur(r->droite));
}

void supprimer(Noeud* r) {
    if (!r) return;
    supprimer(r->gauche);
    supprimer(r->droite);
    delete r;
}

struct Resultat {
    double tempsMoyenUs; // temps moyen pour inserer les N valeurs
    double tempsMinUs;
    double tempsMaxUs;
    double comparaisons; // moyenne
    double hauteurMoy;
};

// Construit l'arbre a partir de 'valeurs' et mesure le temps
Resultat mesurer(int repetitions, bool aleatoire, int N, mt19937& gen) {
    vector<int> valeurs(N);
    for (int i = 0; i < N; i++) valeurs[i] = i + 1;

    double somme = 0, tmin = 1e18, tmax = 0;
    double sommeComp = 0, sommeH = 0;

    for (int r = 0; r < repetitions; r++) {
        if (aleatoire) shuffle(valeurs.begin(), valeurs.end(), gen);

        Noeud* racine = nullptr;
        long long comp = 0;

        auto debut = high_resolution_clock::now();
        for (int v : valeurs) comp += inserer(racine, v);
        auto fin = high_resolution_clock::now();

        double us = duration<double, micro>(fin - debut).count();
        somme += us;
        tmin = min(tmin, us);
        tmax = max(tmax, us);
        sommeComp += comp;
        sommeH += hauteur(racine);
        supprimer(racine);
    }
    return {somme / repetitions, tmin, tmax,
            sommeComp / repetitions, sommeH / repetitions};
}

int main() {
    const int N = 100;
    const int REPETITIONS = 10000;
    mt19937 gen(42);

    // Echauffement
    mesurer(200, false, N, gen);

    Resultat ordre = mesurer(REPETITIONS, false, N, gen);
    Resultat alea  = mesurer(REPETITIONS, true,  N, gen);

    cout << fixed << setprecision(3);
    cout << "N = " << N << ", repetitions = " << REPETITIONS << "\n\n";
    cout << "Cas        | hauteur | comparaisons | tps moyen (us) | min (us) | max (us)\n";
    cout << "Ordonne    | " << ordre.hauteurMoy << " | " << ordre.comparaisons
         << " | " << ordre.tempsMoyenUs << " | " << ordre.tempsMinUs
         << " | " << ordre.tempsMaxUs << "\n";
    cout << "Aleatoire  | " << alea.hauteurMoy << " | " << alea.comparaisons
         << " | " << alea.tempsMoyenUs << " | " << alea.tempsMinUs
         << " | " << alea.tempsMaxUs << "\n";
    cout << "\nRapport des temps (ordonne / aleatoire) : "
         << ordre.tempsMoyenUs / alea.tempsMoyenUs << endl;
    return 0;
}
