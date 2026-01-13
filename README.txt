# Projet : Compression Huffman

## 📋 Présentation
**Projet académique** réalisé dans le cadre du cours de structures de données et algorithmes.

**Équipe : HALIMA OSMAN - AYA MESBAHI ** 
**Année académique : 2025–2026  **
**Filière : GLD **  

## 🎯 Objectifs du Projet

### Objectif Principal
Développer un programme en C capable de compresser et décompresser des fichiers en utilisant l'algorithme de codage de Huffman.

### Objectifs Pédagogiques
- Comprendre et implémenter un algorithme de compression sans perte
- Maîtriser les structures de données avancées (arbres binaires, files de priorité)
- Pratiquer la gestion dynamique de la mémoire en C
- Apprendre à manipuler les fichiers binaires
- Développer une interface utilisateur en ligne de commande

## 📁 Structure et Fonctionnalités des Fichiers

### Dossier `include/`
**`bt.h`** : Définit la structure de l'arbre binaire utilisé pour le codage Huffman  
**`huffman.h`** : Déclare toutes les fonctions de l'algorithme de compression/décompression

### Dossier `src/`
**`bt.c`** : Implémente les opérations de base sur les arbres binaires  
**`huffman.c`** : Contient l'algorithme principal de Huffman (fréquences, arbre, codes)  
**`main.c`** : Interface utilisateur avec menu interactif et gestion des fichiers

### Fichiers de Test
**`test_normal.txt`** : Fichier texte standard pour tests généraux  
**`test_single.txt`** : Test du cas limite avec un seul caractère  
**`test_multiple_A.txt`** : Test de compression avec caractères répétés  
**`empty.txt`** : Test des fichiers avec lignes vides  
**`test_vide.txt`** : Test du cas limite fichier complètement vide

### Fichiers Exécutables et Configuration
**`huffman.exe`** : Programme exécutable final  
**`.vscode/`** : Configuration de l'environnement de développement Visual Studio Code

## ⚙️ Fonctionnalités du Programme

### Compression
- Analyse des fréquences des caractères dans le fichier source
- Construction automatique de l'arbre de Huffman optimal
- Génération des codes binaires compressés
- Création du fichier `.huf` compressé avec métadonnées

### Décompression
- Lecture et interprétation des fichiers `.huf`
- Reconstruction de l'arbre de Huffman à partir des données sauvegardées
- Restauration exacte du fichier original

### Gestion des Cas Spéciaux
- Traitement correct des fichiers vides (0 octet)
- Compression optimale des fichiers avec un seul caractère répété
- Support de tous types de fichiers (texte et binaire)

## 🧪 Tests et Validation

Le projet inclut une série complète de fichiers de test pour valider tous les cas d'usage :
- **Cas standard** : Fichier texte normal avec variété de caractères
- **Cas limites** : Fichiers vides, un seul caractère, caractères répétés
- **Validation** : Vérification que la décompression restitue exactement l'original

## 💻 Technologies Utilisées
- **Langage** : C (norme C11)
- **Compilateur** : GCC / TDM-GCC
- **Environnement** : Visual Studio Code
- **Système** : Architecture modulaire avec séparation header/source

## 📊 Résultats Attendus

### Résultats Techniques
- Programme fonctionnel de compression/décompression
- Taux de compression significatif sur les fichiers redondants
- Gestion robuste des erreurs et cas limites
- Code source bien structuré et documenté

### Apprentissages Pédagogiques
- Maîtrise pratique de l'algorithme de Huffman
- Expérience concrète en développement C avancé
- Compréhension des structures de données complexes
- Capacité à traiter des problèmes algorithmiques réels

## 🎓 Compétences Développées

### Compétences Techniques
- Programmation avancée en C
- Implémentation d'algorithmes complexes
- Gestion de la mémoire dynamique
- Manipulation de fichiers binaires

### Compétences Méthodologiques
- Conception modulaire de programmes
- Tests et validation systématique
- Documentation technique
- Résolution de problèmes complexes

---

*Projet académique - Compression de données par codage Huffman*