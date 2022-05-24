# Labor 2: Nachbildung des Blinkers am Automobil

MPRO Labor bei Prof. Dr.‐Ing. Jens Altenburg an der TH-Bingen

## Zielstellung:
Nach den ersten Versuchen zur Tastenabfrage und der Ausgabe von Lichtsignalen soll im nächsten Schritt die Frage der eventgesteuerten Programmierung vertiefend betrachtet werden. Dazu wird das Blinkverhalten eines Automobils programmtechnisch nachgebildet. Die sechs LEDs auf dem Velleman‐Bord werden in zwei Gruppen á 3 LEDs für den Blinker „Rechts“ bzw. „Links“ unterteilt. Die Taster SW2 und SW5 sind die Schaltkontakte des Blinkhebels (vergl. Laborvorlesung). Mit SW1 wird der Warnblink‐Mode aktiviert. Weil es sich um ein Fahrzeug der neuesten Generation in der Premiumkategorie handelt, ist der Blinkvorgang als „Angeberblinken“ zu realisieren. „Angeberblinken“ ist das nacheinander erfolgende Aufleuchten der drei Richtungs‐LEDs der jeweils aktivierten Seite. Bei dem fiktionalen Auto handelt es sich natürlich um ein elektrisch angetriebenes Gefährt. Ein miniaturisierter Thorium‐Atomreaktor ist die Energiequelle für den Antrieb.

## Aufgabenstellung:
- Programmieren Sie die Blinkfunktion so, dass beim kurzen Antippen des Blinkhebels vier vollständige Blinkzyklen (Kurzblinkmode) absolviert werden. Ein Blinkzyklus besteht aus dem nacheinander erfolgenden Aufleuchten der drei Richtungs‐LEDs (eine LED ein, zwei LEDs ein, alle drei LEDs ein, Ausschalten aller LEDs). Die Schaltzeiten betragen 100 ms pro LED und 300 ms für die Pause mit allen ausgeschalteten LEDs. In Summe ist ein Blinkzyklus dann 600 ms lang. Nach den vier Blinkzyklen geht der Blinkmode automatisch in den Ruhemode. Die Sicht auf die LEDs entspricht der Ansicht auf das Fahrzeug von hinten (bzw. von vorn) – es sind also die beiden Abbiegerichtungen sichtbar.

- Wird innerhalb des Kurzblinkmode der andere Richtungstaster betätigt, beendet dieser Tastendruck den Blinkzyklus sofort, ohne den Richtungsblinker der anderen Seite zu starten. Sofort heißt in diesem Fall, unmittelbares Abschalten aller LEDs!

- Bei einem langen Tastendruck (lang >= 600 ms) wird der Blinkmode in den Dauermode versetzt. In diesem Zustand verbleibt der Blinker bis die Richtungstaste wieder losgelassen wird. Erster Hinweis, in diesem Fall wird die andere Richtungstaste ignoriert, da bei einem realen Blinkhebel nicht beide Tasten gleichzeitig gedrückt werden können. Zweiter Hinweis, nach dem Loslassen des Tasters wird der Blinkzustand noch vollständig abgearbeitet, bevor ausgeschaltet wird, d.h. ist z.B. erst eine LED eingeschaltet, werden die beiden anderen LEDs noch hinzugeschaltet, bevor der Blinkmode dann deaktiviert wird.

- Warnblinker – wenn der Schalter SW1 aktiviert wird geht der Blinker in den Warnblinkmode. In diesem Fall blinken vier LEDs. Die Sicht auf die LEDs entspricht der Sicht von oben auf das Fahrzeug. Warnblinkzeit 300 ms ein, 300 ms aus. Das Deaktivieren von SW1 stoppt den Warnblinker. Abgeschaltet wird immer in der „Aus‐Phase“ des Blinkers.

- Diagnosefunktion – zum exakten Test des Blinkverhaltens werden die Blinkzeiten im Diagnosemode verdreifacht. Anstelle von 100 ms als kürzester Zeit wird jetzt mit 300 ms gearbeitet. Der Diagnosemode ist in der Software implementiert und wird durch Drücken der Taste SW3 nach dem POR aktiviert. Der Diagnosemode wird erst nach einem erneuten POR verlassen

## Lösungshinweise:
Versuchen Sie die einzelnen Funktionen in ein festes Zeitraster zu bringen, Stichwort „kooperatives Multitasking“. Nutzen Sie Zustandsautomaten und eventorientierte Programmierung. Ein Besuch der Laborvorlesung ist hilfreich.