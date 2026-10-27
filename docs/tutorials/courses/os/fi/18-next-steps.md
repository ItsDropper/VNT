# Miten jatkaa kurssin jälkeen

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Valitse yksi alijärjestelmä ja seuraa sitä toteutuksesta testeihin. Hyviä aiheita ovat keskeytykset, fyysisen muistin varaaja, ACPI ja APIC. Älä aloita suurta uudelleenkirjoitusta ennen kuin pystyt selittämään nykyisen toiminnan ja sen rajoitukset.

## Sanasto

Alijärjestelmä on rajattu osa. Rajapinta määrittää, miten sitä kutsutaan. Invariantti on sääntö, jonka pitää pysyä totena, kuten ettei samaa vapaata sivukehystä anneta kahdelle käyttäjälle.

## Käytännön harjoitus

Valitse aihe, etsi toteutustiedosto, kaikki kutsujat ja testit. Kirjoita kartta: mitä dataa osa omistaa, mitä funktioita se tarjoaa ja mikä voi mennä vikaan. Tee vasta sitten rajattu muutos.

## Yleinen virhe ja turvallisuus

Ominaisuuden lisääminen vaatii usein virhetilanteiden käsittelyä, testejä ja rajapinnan suunnittelua. Moniydinkoodissa pitää lisäksi miettiä samanaikainen käyttö.

## Tarkista ymmärtäminen

Valitse jatkoaihe ja nimeä kolme asiaa, jotka tutkit ennen koodaamista.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Tee ensimmäinen pieni muutos](17-first-kernel-change.md) · [Seuraava oppitunti: Jaa kernel VNT-moduuleihin →](19-split-the-kernel-into-vnt-modules.md)
