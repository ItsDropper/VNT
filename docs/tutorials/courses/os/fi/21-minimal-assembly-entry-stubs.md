# Pidä assembly-rajapinta pienenä

Copyright (c) 2026 ItsDropper. MIT-lisenssi; katso projektin juuren LICENSE.

## Tavoite ja selitys

Assembly on tarpeen arkkitehtuurin rajakohdissa, mutta siitä ei pidä tehdä koko kernelin kieltä. Keskeytyksen sisääntulo voi tallentaa rekisterit ja valmistella tilan ennen VNT-funktiokutsua. Tapahtuman varsinainen käsittely kuuluu yleensä VNT-koodiin.

## Sanasto

Stub on pieni välityskoodi. Calling convention määrittää argumenttien, rekisterien ja paluuarvojen käytön. Clobber tarkoittaa rekisteriä tai tilaa, jonka koodi voi muuttaa.

## Käytännön harjoitus

Lue os/arch/x86_64/interrupts.S. Kirjaa, mitä tehdään ennen VNT-käsittelijää ja sen jälkeen. Selvitä pinon muutokset ja tallennetut rekisterit. Vertaa VNT-funktion odottamaan tietorakenteeseen.

## Yleinen virhe ja turvallisuus

Inline-assembly ilman kääntäjälle ilmoitettuja muistivaikutuksia voi johtaa virheelliseen koodiin. Käytä määriteltyä ja testattua kääntäjärajapintaa, älä irrallista assembly-tekstiä synkronoinnin korvikkeena.

## Tarkista ymmärtäminen

Nimeä yksi tehtävä, joka vaatii assemblya, ja yksi, joka kuuluu VNT-koodiin.

## Ennen kuin jatkat

Varmista, että osaat selittää tämän oppitunnin pääkäsitteen omin sanoin, nimetä lähdekoodin kohdan, jossa se näkyy, ja kertoa, millä havainnolla testaisit toimintaa. Jos et osaa, palaa tiedostoon ja seuraa koodia vielä kerran.

---

[← Sisällysluettelo](./README.md) · [← Edellinen: Keskeytykset, poikkeukset ja IDT](20-interrupts-and-the-idt.md) · [Seuraava oppitunti: Käsittele keskeytyksiä VNT-kielellä →](22-vnt-interrupt-dispatch.md)
