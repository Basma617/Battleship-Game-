// Inclusion des bibliothèques nécessaires
#include <SDL3/SDL.h>   // Bibliothèque graphique SDL3 (peut-être utilisé pour l'affichage des scores)
#include <stdio.h>      // Bibliothèque standard d'entrée/sortie (fopen, fprintf, fscanf, etc.)
#include "types.h"      // Définitions des types personnalisés (Score, ScoreMode, etc.)
#include "files.h"      // Déclarations des fonctions de gestion de fichiers (prototypes)

// ============================================================================
// FONCTION : getScoreTag
// DESCRIPTION : Convertit un mode de score en chaîne de caractères (étiquette)
// PARAMÈTRES :
//   - mode : le mode de difficulté (ScoreMode enum)
// RETOUR : Chaîne de caractères constante représentant le mode
// ============================================================================
const char *getScoreTag(ScoreMode mode) {
    // Instruction switch pour traiter chaque valeur possible de l'énumération ScoreMode
    switch (mode) {
        case SCORE_EASY:   return "[EASY]";    // Mode facile contre IA
        case SCORE_MEDIUM: return "[MEDIUM]";  // Mode moyen contre IA
        case SCORE_HARD:   return "[HARD]";    // Mode difficile contre IA
        case SCORE_PVP:    return "[PVP]";        // Mode joueur contre joueur
        default:           return "[UNKNOWN]";    // Mode non reconnu (valeur par défaut)
    }
}

// ============================================================================
// FONCTION : saveScore
// DESCRIPTION : Sauvegarde un score dans un fichier au format simple
// PARAMÈTRES :
//   - filename   : nom du fichier où sauvegarder
//   - playerName : nom du joueur
//   - score      : score à sauvegarder
// RETOUR : aucun (void)
// ============================================================================
void saveScore(const char *filename, const char *playerName, int score) {
    // Ouvre le fichier en mode "append" (ajout à la fin)
    FILE *f = fopen(filename, "a");
    
    // Vérifie si l'ouverture a réussi
    if (!f) {
        printf("Erreur ouverture fichier scores\n");  // Message d'erreur en console
        return;  // Quitte la fonction en cas d'échec
    }

    // Écrit le nom et le score dans le fichier au format "Nom Score"
    fprintf(f, "%s %d\n", playerName, score);
    
    // Ferme le fichier pour libérer les ressources
    fclose(f);
}

// ============================================================================
// FONCTION : loadScores
// DESCRIPTION : Charge et affiche tous les scores d'un fichier (version console)
// PARAMÈTRES :
//   - filename : nom du fichier à lire
// RETOUR : aucun (void)
// ============================================================================
void loadScores(const char *filename) {
    // Ouvre le fichier en mode "read" (lecture seule)
    FILE *f = fopen(filename, "r");
    
    // Variables pour stocker temporairement les données lues
    char name[50];  // Buffer pour le nom du joueur (49 caractères max + terminateur)
    int score;      // Variable pour le score
    
    // Vérifie si l'ouverture a réussi
    if (!f) {
        printf("Aucun fichier de scores trouvé.\n");  // Message si fichier inexistant
        return;  // Quitte la fonction
    }

    // Affiche un en-tête dans la console
    printf("=== Meilleurs scores ===\n");
    
    // Lit le fichier ligne par ligne jusqu'à la fin
    // fscanf retourne 2 si les 2 valeurs (nom et score) sont correctement lues
    while (fscanf(f, "%49s %d", name, &score) == 2) {
        // Affiche chaque score dans la console
        printf("%s : %d\n", name, score);
    }

    // Ferme le fichier
    fclose(f);
}

// ============================================================================
// FONCTION : loadScoresSection
// DESCRIPTION : Charge les scores d'une section spécifique (difficulté) depuis un fichier structuré
// PARAMÈTRES :
//   - filename : nom du fichier à lire
//   - mode     : mode de difficulté (détermine quelle section charger)
//   - scores   : tableau où stocker les scores chargés
// RETOUR : nombre de scores chargés (int)
// ============================================================================
int loadScoresSection(const char *filename, ScoreMode mode, Score scores[]) {
    // Ouvre le fichier en mode lecture
    FILE *f = fopen(filename, "r");
    
    // Buffer pour lire les lignes du fichier
    char line[128];
    
    // Compteur de scores chargés
    int count = 0;
    
    // Récupère l'étiquette correspondant au mode (ex: "[IA EASY]")
    const char *tag = getScoreTag(mode);

    // Vérifie si l'ouverture a réussi
    if (!f) return 0;  // Retourne 0 si fichier inexistant

    // Parcourt le fichier ligne par ligne
    while (fgets(line, sizeof(line), f)) {
        // Compare le début de la ligne avec l'étiquette recherchée
        if (strncmp(line, tag, strlen(tag)) == 0) {
            // Section trouvée ! Lit les scores de cette section
            while (fgets(line, sizeof(line), f)) {
                // Si la ligne commence par '[', c'est une nouvelle section -> on arrête
                if (line[0] == '[') break;
                
                // Essaie de parser la ligne au format "Nom Score"
                // sscanf retourne 2 si les 2 valeurs sont correctement extraites
                if (sscanf(line, "%49s %d",
                           scores[count].name,        // Stocke le nom dans le tableau
                           &scores[count].score) == 2) {  // Stocke le score dans le tableau
                    count++;  // Incrémente le compteur
                    
                    // Limite le nombre de scores chargés à MAX_SCORES
                    if (count >= MAX_SCORES) break;
                }
            }
            // Sort de la boucle while principale car section trouvée
            break;
        }
    }

    // Ferme le fichier
    fclose(f);
    
    // Retourne le nombre de scores chargés
    return count;
}

// ============================================================================
// FONCTION : sortScores
// DESCRIPTION : Trie un tableau de scores par ordre décroissant (tri à bulles)
// PARAMÈTRES :
//   - scores : tableau de scores à trier
//   - count  : nombre d'éléments dans le tableau
// RETOUR : aucun (void)
// ============================================================================
void sortScores(Score scores[], int count) {
    // Boucle externe : parcourt le tableau
    for (int i = 0; i < count - 1; i++)
        // Boucle interne : compare l'élément i avec tous les suivants
        for (int j = i + 1; j < count; j++)
            // Si le score à la position j est supérieur à celui à la position i
            if (scores[j].score > scores[i].score) {
                // Échange les deux scores (tri décroissant)
                Score tmp = scores[i];      // Sauvegarde temporaire de scores[i]
                scores[i] = scores[j];      // Remplace scores[i] par scores[j]
                scores[j] = tmp;            // Remplace scores[j] par la sauvegarde
            }
}

// ============================================================================
// FONCTION : writeAllScores
// DESCRIPTION : Écrit tous les scores dans un fichier structuré (toutes difficultés)
// PARAMÈTRES :
//   - filename : nom du fichier où écrire
//   - easy[]   : tableau des scores en mode facile
//   - ne       : nombre de scores en mode facile
//   - medium[] : tableau des scores en mode moyen
//   - nm       : nombre de scores en mode moyen
//   - hard[]   : tableau des scores en mode difficile
//   - nh       : nombre de scores en mode difficile
//   - pvp[]    : tableau des scores en mode PVP
//   - np       : nombre de scores en mode PVP
// RETOUR : aucun (void)
// ============================================================================
void writeAllScores(const char *filename,
                    Score easy[], int ne,
                    Score medium[], int nm,
                    Score hard[], int nh,
                    Score pvp[], int np) {

    // Ouvre le fichier en mode "write" (écriture, écrase le contenu existant)
    FILE *f = fopen(filename, "w");
    
    // Vérifie si l'ouverture a réussi
    if (!f) return;  // Quitte si échec

    // Écrit la section EASY
    fprintf(f, "[EASY]\n");
    for (int i = 0; i < ne; i++)
        fprintf(f, "%s %d\n", easy[i].name, easy[i].score);

    // Ligne vide pour séparer les sections
    fprintf(f, "\n[MEDIUM]\n");
    for (int i = 0; i < nm; i++)
        fprintf(f, "%s %d\n", medium[i].name, medium[i].score);

    // Ligne vide pour séparer les sections
    fprintf(f, "\n[HARD]\n");
    for (int i = 0; i < nh; i++)
        fprintf(f, "%s %d\n", hard[i].name, hard[i].score);

    // Ligne vide pour séparer les sections
    fprintf(f, "\n[PVP]\n");
    for (int i = 0; i < np; i++)
        fprintf(f, "%s %d\n", pvp[i].name, pvp[i].score);

    // Ferme le fichier
    fclose(f);
}

// ============================================================================
// FONCTION : updateTopScoresOneFile
// DESCRIPTION : Met à jour les meilleurs scores dans un fichier unique
//               Ajoute un nouveau score, trie et limite à MAX_SCORES
// PARAMÈTRES :
//   - filename   : nom du fichier de scores
//   - mode       : mode de difficulté du nouveau score
//   - playerName : nom du joueur
//   - playerScore: score du joueur
// RETOUR : aucun (void)
// ============================================================================
void updateTopScoresOneFile(const char *filename,
                            ScoreMode mode,
                            const char *playerName,
                            int playerScore) {

    // Déclaration des tableaux pour chaque catégorie de score
    // +1 pour pouvoir ajouter le nouveau score avant de trier
    Score easy[MAX_SCORES + 1], medium[MAX_SCORES + 1];
    Score hard[MAX_SCORES + 1], pvp[MAX_SCORES + 1];

    // Charge les scores existants pour chaque catégorie
    int ne = loadScoresSection(filename, SCORE_EASY, easy);    // Scores facile
    int nm = loadScoresSection(filename, SCORE_MEDIUM, medium); // Scores moyen
    int nh = loadScoresSection(filename, SCORE_HARD, hard);    // Scores difficile
    int np = loadScoresSection(filename, SCORE_PVP, pvp);      // Scores PVP

    // Pointeurs vers le tableau et compteur appropriés selon le mode
    Score *current;  // Pointeur vers le tableau de la bonne catégorie
    int *count;      // Pointeur vers le compteur de la bonne catégorie

    // Sélectionne la catégorie appropriée selon le mode
    switch (mode) {
        case SCORE_EASY:   current = easy;   count = &ne; break;   // Mode facile
        case SCORE_MEDIUM: current = medium; count = &nm; break;   // Mode moyen
        case SCORE_HARD:   current = hard;   count = &nh; break;   // Mode difficile
        case SCORE_PVP:    current = pvp;    count = &np; break;   // Mode PVP
        default: return;  // Mode non reconnu, quitte la fonction
    }

    // Ajoute le nouveau score à la fin du tableau sélectionné
    strcpy(current[*count].name, playerName);  // Copie le nom du joueur
    current[*count].score = playerScore;       // Affecte le score
    (*count)++;  // Incrémente le compteur

    // Trie les scores de la catégorie (ordre décroissant)
    sortScores(current, *count);
    
    // Limite le nombre de scores à MAX_SCORES (garder seulement les meilleurs)
    if (*count > MAX_SCORES) *count = MAX_SCORES;

    // Réécrit tout le fichier avec les scores mis à jour
    writeAllScores(filename, easy, ne, medium, nm, hard, nh, pvp, np);
}

// ============================================================================
// FONCTION : getBestScore
// DESCRIPTION : Récupère le meilleur score pour une difficulté donnée
// PARAMÈTRES :
//   - filename : nom du fichier de scores
//   - mode     : mode de difficulté
// RETOUR : le meilleur score (int), ou 0 si aucun score
// ============================================================================
int getBestScore(const char *filename, ScoreMode mode) {
    // Tableau temporaire pour stocker les scores
    Score scores[MAX_SCORES];
    
    // Charge les scores de la section demandée
    int count = loadScoresSection(filename, mode, scores);
    
    // Si aucun score n'a été chargé, retourne 0
    if (count == 0) return 0;
    
    // Trie les scores par ordre décroissant
    sortScores(scores, count);
    
    // Retourne le premier score (le meilleur après tri)
    return scores[0].score;
}