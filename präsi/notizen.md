#### Einleitung

- ggf Minecraft kurz erklären
- generelle Infos übers projekt in Originalform


#### Domänenmodell

- kurz idee der struktur erklären des spiel

#### Designmodell

- umsetzung der struktur erklären
- alle einzelne bereiche des Modells grob zusammenfassen
- ins detail wo design wichtiger ist eg Spieler, Welt, Blöcke und Generation

#### Änderungen

- Änderungen kurz erklären und relevante änderungen an modellen erklären

##### Movement

- use cases erklären und übergang zu änderungen
###### Use-Cases
- zustände movement
- use cases alle kurz zusammenfassen und erklären

- überarbeitung movement formeln nach minecraft wiki
- delta zu tick
- interpolation
- anpassung kollision mit lokalem hilfsobjekt + sneak
- raycasting

##### Strukturen

##### GUI & Inventar

- OpenGL & SFML kurz erwähnen
- Debug Info kurz erläutern (warum entstanden -> eigentlich zum SFML und Player debugen im zusammenhang mit FPS counter, Movement und Inventar)
- Inventar Klasse erläutern (funktion, nutzen, nicht detaliert methoden, vorher nacher)
- Inventar Klasse einordnen (teil von Player Klasse, kann mit I-Key toggled werden)

#### Vorstellung

- ui vorstellen (inventar demonstrieren, abbauen, platzieren, debug)
- strukturen zeigen
- block slipperiness zeigen und block bounce zeigen
- movement nur erläutern oder evtl zwei versionen zeigen (tendenz rauslassen wenn zeit felt)

#### Fazit

- Codebase & Pflege (zu viel, unmodular, fehlende erklärung -> unübersichtlich)
- OpenGL (schwierigkeiten mit 2D-Rendering über 3D-Rendering, GLStates richtig verlassen)
- Buildprozess (fehlende schritte für Linux-Distributionen -> Recherche -> nimmt Zeit in anspruch)
- Versionsverwaltung mit Git (effizient, übersichtlich <- vorallem mit mehreren Personen)
- Diagramme (Vertiefung von Verständnis für Code für Entwickler und Dritte)

- Abschluss (erlangen und vertiefen von Wissen mit CMake, OpenGL und dem Arbeiten in einer Gruppe an Software) 
- ggf. offene Fragen klären