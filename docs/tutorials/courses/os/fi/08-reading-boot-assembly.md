# Lue käynnistys-assemblya

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Assembly näyttää CPU:n käskyt ja rekisterien käytön suoraan. Se on hyödyllinen, kun asetetaan ohjausrekistereitä tai käsitellään pinoa. Yhden käskyn merkitys riippuu rekisterien aiemmista arvoista ja CPU:n nykyisestä tilasta.

## Sanasto

Rekisteri on CPU:n sisäinen tallennuspaikka. Label nimeää koodikohdan. Hyppy muuttaa suorituksen seuraavaa osoitetta. Call ja ret liittyvät funktiokutsuun ja paluuseen.

## Käytännön harjoitus

Lue os/arch/x86_64/boot.S 5–10 riviä kerrallaan. Kirjoita jokaisesta rivistä, mitä rekisteriä tai muistia käytetään ja mihin suoritus jatkuu. Etsi käskyt mov, jmp, call ja ret ja selvitä niiden käyttö tässä yhteydessä.

## Yleinen virhe ja turvallisuus

Pidä assembly pienenä. Tavallinen päätöksenteko kannattaa tehdä VNT-kielessä, jos se on mahdollista turvallisesti.

## Tarkista ymmärtäminen

Valitse viisi käskyä ja selitä niiden vaikutus juuri tässä koodissa.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.


## Syvempi harjoitus: seuraa koodia vaiheittain

Älä lue assemblya vain käsky kerrallaan. Seuraa rekisterien arvoja ennen käskyä ja sen jälkeen. Kirjoita esimerkiksi rekisterin arvo ennen mov-käskyä ja sen jälkeen.

Tee harjoitus näin:
1. **Ennusta:** kirjoita yhdellä lauseella, mitä uskot tämän osan tekevän.
2. **Etsi lähtökohta:** etsi keskeinen funktio tai käsky ja selvitä, kuka sitä kutsuu.
3. **Seuraa tietoa:** kirjaa, mitä arvoja funktio saa, mitä tilaa se muuttaa ja mitä tapahtuu seuraavaksi.
4. **Etsi virhepolku:** mitä jos tieto puuttuu, laite ei vastaa tai oletettu tila ei olekaan voimassa?
5. **Todenna:** määritä näkyvä havainto, joka osoittaisi toiminnon tapahtuneen. Pelkkä se, ettei kernel kaadu heti, ei ole riittävä testi.
6. **Tarkista oletus:** vertaa lähdekoodia alkuperäiseen ennustukseesi ja korjaa muistiinpanoja.

Kirjoita lopuksi omaan sanastoosi uusi käsite, sen selitys ja yksi konkreettinen esimerkki tästä tiedostosta. Kernelin ymmärtäminen ei ole käskyjen ulkoa muistamista: tärkeintä on hahmottaa osan vastuu, rajapinta, riippuvuudet ja rajoitukset.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: GRUB ja Multiboot2](07-grub-and-multiboot2.md) · [Seuraava oppitunti: x86-64 long mode →](09-long-mode.md)
