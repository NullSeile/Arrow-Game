#include "UserInterface.h"

static void Reset(Rocket &r, World &world)
{
	world.levelImg.loadFromFile(world.ImgPath);
	world.levelTex.loadFromFile(world.ImgPath);
	world.levelSpr.setTexture(world.levelTex);
	r.drawable.setFillColor(world.rocketColor);
	r.acc = { 0, 0 };
	r.vel = { 0, 0 };
	r.pos = { world.iX, world.iY };
	r.drawable.setRotation(world.rotation);
}

UserInterface::UserInterface(sf::RenderWindow &window, Rocket &r, Controller &ctr)
{
	m_font.loadFromFile("res/font/font.ttf");

	ResetUi(window, r, ctr);
}

void UserInterface::ResetUi(sf::RenderWindow &window, Rocket &r, Controller &ctr)
{
	// Clear everything
	m_menu.Clear();
	m_pause.Clear();
	m_play.Clear();
	m_finish.Clear();

	////////////////////
	// MENU
	////////////////////
	// Sprites
	sf::Texture* menuImg_t = new sf::Texture;
	menuImg_t->loadFromFile("res/img/menu.png");

	ui::Sprite* menuImg = new ui::Sprite("menuImg");
	menuImg->setTexture(*menuImg_t);
	menuImg->setOrigin(sf::Vector2f(menuImg->getLocalBounds().width / 2.f, menuImg->getLocalBounds().height / 2.f));

	menuImg->setUpdateFunction([&](ui::Sprite *self)
	{
		self->setScale(((float)window.getSize().x / (float)self->getTexture()->getSize().x) / 1.1f, ((float)window.getSize().y / (float)self->getTexture()->getSize().y) / 1.1f);

		self->setPosition(window.mapPixelToCoords(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2)));
	});
	m_menu.AddSprite(menuImg);

	// Texts
	ui::Text* title = new ui::Text("title");
	title->setFont(m_font);
	title->setOutlineColor(sf::Color::Black);
	title->setOutlineThickness(2);
	title->setString("Arrow Game");
	title->setCharacterSize(80);

	title->setUpdateFunction([&](ui::Text *self)
	{
		self->setPosition(window.mapPixelToCoords(sf::Vector2i((int)(window.getSize().x - self->getGlobalBounds().width - 80), (int)(window.getSize().y - self->getGlobalBounds().height * 1.5f - 10))));
	});
	m_menu.AddText(title);

	// Buttons
	ui::Button *resB = new ui::Button("Reset");
	resB->text.setFont(m_font);
	resB->text.setString("Reset progress");
	resB->text.setCharacterSize(20);
	resB->shape.setSize(sf::Vector2f(resB->text.getGlobalBounds().width + 20, 30));

	resB->setUpdateFunction([&](ui::Button *self)
	{
		self->shape.setPosition(window.mapPixelToCoords(sf::Vector2i(10, window.getSize().y - 40)));
	});

	resB->setClickFunction([&](ui::Button *self)
	{
		if (MessageBox(NULL, "Are you sure?", "WARNING", MB_YESNO) == IDYES)
		{
			for (World &w : ctr.worlds)
			{
				w.able = w.index != 0 ? false : true;
				w.completed = 0;
				w.record = NO_RECORD;
			}

			ResetUi(window, r, ctr);
		}
	});
	m_menu.AddButton(resB);


	sf::Vector2f pos(10, 10);

	for (World &w : ctr.worlds)
	{
		ui::Button* b = new ui::Button(w.id);
		b->text.setFont(m_font);
		b->text.setString(w.id);
		b->text.setCharacterSize(20);
		b->shape.setSize(sf::Vector2f(b->text.getLocalBounds().width + 20, 30));
		b->setAble(w.able);

		if (pos.x + b->shape.getGlobalBounds().width * 1.1 >= window.getSize().x)
		{
			pos.y += 40;
			pos.x = 10;
		}

		b->setUpdateFunction([&, pos](ui::Button *self)
		{
			self->shape.setPosition(window.mapPixelToCoords(sf::Vector2i((int)pos.x, (int)pos.y)));

			if (w.completed = 1)
			{
				if		(w.record < w.time.goldT)		self->shape.setFillColor({ 255, 200, 000 });
				else if (w.record < w.time.silverT)		self->shape.setFillColor({ 210, 210, 210 });
				else if (w.record < w.time.bronzeT)		self->shape.setFillColor({ 165, 113, 100 });
				else if (w.record < w.time.maxT)		self->shape.setFillColor({ 255, 255, 255 });
				else									self->shape.setFillColor({ 255, 255, 255 });
			}
		});

		b->setClickFunction([&](ui::Button *self)
		{
			for (World &w : ctr.worlds)
			{
				if (self->id == w.id)
				{
					ctr.state = State::Pause;
					ctr.level = w.index;
					ctr.cWorld = &w;

					Reset(r, *ctr.cWorld);

					ctr.cWorld->timer.restart();
				}
			}
		});

		m_menu.AddButton(b);

		pos.x += b->shape.getGlobalBounds().width * 1.1f + 5;
	}

	/////////////////////////
	// OPTIONS
	/////////////////////////

	/////////////////////////
	// PAUSE
	/////////////////////////
	// Sprites
	sf::Texture* rewardsImg_t = new sf::Texture;
	rewardsImg_t->loadFromFile("res/img/Pause.png");

	ui::Sprite *rewardsImg = new ui::Sprite("rewards");
	rewardsImg->setTexture(*rewardsImg_t);
	rewardsImg->setScale(0.7f, 0.7f);
	rewardsImg->setOrigin(sf::Vector2f(rewardsImg->getLocalBounds().width / 2.f, rewardsImg->getLocalBounds().height / 2.f));

	rewardsImg->setUpdateFunction([&](ui::Sprite *self)
	{
		self->setPosition(window.mapPixelToCoords(sf::Vector2i(window.getSize().x / 2, window.getSize().y - 70)));
	});
	m_pause.AddSprite(rewardsImg);

	// Texts
	ui::Text *levelTitleP = new ui::Text("levelTitleP");
	levelTitleP->setFont(m_font);
	levelTitleP->setCharacterSize(40);
	levelTitleP->setOutlineThickness(1);
	levelTitleP->setOutlineColor(sf::Color::Black);

	levelTitleP->setUpdateFunction([&](ui::Text *self)
	{
		std::stringstream lTitleText;
		lTitleText << ctr.cWorld->id;

		self->setString(lTitleText.str().c_str());
		self->setOrigin(sf::Vector2f(self->getLocalBounds().width / 2.f, 0));

		self->setPosition(window.mapPixelToCoords(sf::Vector2i(window.getSize().x / 2, 10)));
	});
	m_pause.AddText(levelTitleP);

	ui::Text *goldT = new ui::Text("goldT");
	goldT->setFont(m_font);
	goldT->setFillColor(sf::Color::Black);
	goldT->setCharacterSize(22);

	goldT->setUpdateFunction([&](ui::Text *self)
	{
		std::stringstream goldT_t;
		goldT_t << std::fixed << std::setprecision(3) << ctr.cWorld->time.goldT << "s";

		self->setString(goldT_t.str().c_str());

		self->setPosition(window.mapPixelToCoords(sf::Vector2i((int)(window.getSize().x / 2) - 260, (int)(window.getSize().y) - 90)));
	});
	m_pause.AddText(goldT);

	ui::Text *silverT = new ui::Text("silverT");
	silverT->setFont(m_font);
	silverT->setFillColor(sf::Color::Black);
	silverT->setCharacterSize(22);

	silverT->setUpdateFunction([&](ui::Text *self)
	{
		std::stringstream silverT_t;
		silverT_t << std::fixed << std::setprecision(3) << ctr.cWorld->time.silverT << "s";

		self->setString(silverT_t.str().c_str());

		self->setPosition(window.mapPixelToCoords(sf::Vector2i((int)(window.getSize().x / 2) - 25, (int)(window.getSize().y) - 90)));
	});
	m_pause.AddText(silverT);

	ui::Text *bronzeT = new ui::Text("bronzeT");
	bronzeT->setFont(m_font);
	bronzeT->setFillColor(sf::Color::Black);
	bronzeT->setCharacterSize(22);

	bronzeT->setUpdateFunction([&](ui::Text *self)
	{
		std::stringstream bronzeT_t;
		bronzeT_t << std::fixed << std::setprecision(3) << ctr.cWorld->time.bronzeT << "s";

		self->setString(bronzeT_t.str().c_str());

		self->setPosition(window.mapPixelToCoords(sf::Vector2i((int)(window.getSize().x / 2) + 210, (int)(window.getSize().y) - 90)));
	});
	m_pause.AddText(bronzeT);

	ui::Text *recordT = new ui::Text("recordT");
	recordT->setFont(m_font);
	recordT->setFillColor(sf::Color::Black);
	recordT->setOutlineThickness(1);
	recordT->setOutlineColor(sf::Color::White);
	recordT->setCharacterSize(20);

	recordT->setUpdateFunction([&](ui::Text *self)
	{
		std::stringstream t_record;
		if ((int)ctr.cWorld->record != NO_RECORD)
		{
			t_record << std::fixed << std::setprecision(3) << "Record:\n" << ctr.cWorld->record << "s";
		}
		else
		{
			t_record << std::fixed << std::setprecision(3) << "Record:\n--.---";
		}

		self->setString(t_record.str().c_str());

		self->setPosition(window.mapPixelToCoords(sf::Vector2i((int)(window.getSize().x / 2) - 430, (int)(window.getSize().y) - 95)));
	});
	m_pause.AddText(recordT);

	/////////////////
	// Playing
	/////////////////
	// Texts
	ui::Text *currentT = new ui::Text("currentT");
	currentT->setFont(m_font);
	currentT->setOutlineThickness(2);
	currentT->setOutlineColor(sf::Color::White);

	currentT->setUpdateFunction([&](ui::Text *self)
	{
		if		(ctr.cWorld->currentT < ctr.cWorld->time.goldT)		self->setFillColor({ 255, 200,   0 });
		else if (ctr.cWorld->currentT < ctr.cWorld->time.silverT)	self->setFillColor({ 210, 210, 210 });
		else if (ctr.cWorld->currentT < ctr.cWorld->time.bronzeT)	self->setFillColor({ 165, 113, 100 });
		else														self->setFillColor({ 255, 255, 255 });
		
		std::stringstream currentT_t;
		currentT_t << std::fixed << std::setprecision(3) << ctr.cWorld->currentT << "s";

		self->setString(currentT_t.str().c_str());

		self->setPosition(window.mapPixelToCoords(sf::Vector2i(5, 5)));
	});
	m_play.AddText(currentT);

	/////////////////
	// End
	/////////////////
	// Sprites
	sf::Texture *winSpr_texture = new sf::Texture;
	winSpr_texture->loadFromFile("res/img/winScreen.png");
	
	ui::Sprite *winSpr = new ui::Sprite("winSpr");
	winSpr->setTexture(*winSpr_texture);
	winSpr->setScale(0.5f, 0.5f);
	winSpr->setOrigin(winSpr->getLocalBounds().width / 2.f, winSpr->getLocalBounds().height / 2.f);

	winSpr->setUpdateFunction([&](ui::Sprite *self)
	{
		self->setPosition(window.mapPixelToCoords(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2)));
	});
	m_finish.AddSprite(winSpr);

	// Texts
	ui::Text *newRecordT = new ui::Text("newRecord");
	newRecordT->setFont(m_font);
	newRecordT->setCharacterSize(30);
	newRecordT->setFillColor(sf::Color::Black);

	newRecordT->setUpdateFunction([&](ui::Text *self)
	{
		if (ctr.cWorld->currentT == ctr.cWorld->record)
		{
			self->setString("NEW RECORD!");
		}
		else
		{
			self->setString("");
		}

		self->setOrigin(self->getLocalBounds().width / 2.f, 0);
		self->setPosition(window.mapPixelToCoords(sf::Vector2i(window.getSize().x / 2, (window.getSize().y / 2) - 55)));
	});
	m_finish.AddText(newRecordT);

	ui::Text *levelTitleF = new ui::Text("levelTitleF");
	levelTitleF->setFont(m_font);
	levelTitleF->setCharacterSize(40);
	levelTitleF->setOutlineThickness(1);
	levelTitleF->setOutlineColor(sf::Color::Black);

	levelTitleF->setUpdateFunction([&](ui::Text *self)
	{
		std::stringstream lTitleText;
		lTitleText << ctr.cWorld->id;

		self->setString(lTitleText.str().c_str());
		self->setOrigin(self->getLocalBounds().width / 2.f, 0);

		self->setPosition(window.mapPixelToCoords(sf::Vector2i(window.getSize().x / 2, (window.getSize().y / 2) - 120)));
	});
	m_finish.AddText(levelTitleF);

	ui::Text *finalT = new ui::Text("finalT");
	finalT->setFont(m_font);
	finalT->setOutlineThickness(1);
	finalT->setOutlineColor(sf::Color::Black);

	finalT->setUpdateFunction([&](ui::Text *self)
	{
		if		(ctr.cWorld->currentT < ctr.cWorld->time.goldT)		self->setFillColor({ 255, 200,   0 });
		else if (ctr.cWorld->currentT < ctr.cWorld->time.silverT)	self->setFillColor({ 210, 210, 210 });
		else if (ctr.cWorld->currentT < ctr.cWorld->time.bronzeT)	self->setFillColor({ 165, 113, 100 });
		else														self->setFillColor({ 255, 255, 255 });

		std::stringstream finalT_t;
		finalT_t << std::fixed << std::setprecision(3) << ctr.cWorld->currentT << "s";

		self->setString(finalT_t.str().c_str());

		self->setOrigin(self->getLocalBounds().width / 2, self->getLocalBounds().height / 2);

		self->setPosition(window.mapPixelToCoords(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2)));
	});
	m_finish.AddText(finalT);

	// BUTTONS
	ui::Button *nextB = new ui::Button("nextB");
	nextB->text.setFont(m_font);
	nextB->text.setString("Next Level\n[SPACE]");
	nextB->text.setCharacterSize(15);
	nextB->shape.setSize(sf::Vector2f(100, 45));

	nextB->setClickFunction([&](ui::Button *self)
	{
		if (ctr.worlds[ctr.level].NextLevel != "NULL")
		{
			ctr.state = State::Pause;
			ctr.level++;
			ctr.cWorld = &ctr.worlds[ctr.level];
			Reset(r, *ctr.cWorld);
			ctr.cWorld->timer.restart();
		}
	});

	nextB->setUpdateFunction([&](ui::Button *self)
	{
		self->shape.setPosition(window.mapPixelToCoords(sf::Vector2i((window.getSize().x / 2) + 97, (window.getSize().y / 2) + 78)));
		self->text.setPosition(self->shape.getPosition().x + 5, self->shape.getPosition().y + 2);
	});
	m_finish.AddButton(nextB);

	ui::Button *resetB = new ui::Button("resetB");
	resetB->text.setFont(m_font);
	resetB->text.setString("Restart\n[R]");
	resetB->text.setCharacterSize(15);
	resetB->shape.setSize(sf::Vector2f(100, 45));

	resetB->setClickFunction([&](ui::Button *self)
	{
		ctr.state = State::Pause;
		Reset(r, *ctr.cWorld);
		ctr.cWorld->timer.restart();
	});

	resetB->setUpdateFunction([&](ui::Button *self)
	{
		self->shape.setPosition(window.mapPixelToCoords(sf::Vector2i((window.getSize().x / 2) - 50, (window.getSize().y / 2) + 78)));
		self->text.setPosition(self->shape.getPosition().x + 5, self->shape.getPosition().y + 2);
	});
	m_finish.AddButton(resetB);

	ui::Button *menuB = new ui::Button("menuB");
	menuB->text.setFont(m_font);
	menuB->text.setString("Menu\n[M]");
	menuB->text.setCharacterSize(15);
	menuB->shape.setSize(sf::Vector2f(100, 45));

	menuB->setClickFunction([&](ui::Button *self)
	{
		ctr.state = State::Menu;
		Reset(r, *ctr.cWorld);
		ResetUi(window, r, ctr);
	});

	menuB->setUpdateFunction([&](ui::Button *self)
	{
		self->shape.setPosition(window.mapPixelToCoords(sf::Vector2i((window.getSize().x / 2) - 205, (window.getSize().y / 2) + 78)));
		self->text.setPosition(self->shape.getPosition().x + 5, self->shape.getPosition().y + 2);
	});
	m_finish.AddButton(menuB);
}

void UserInterface::Update(const State &state, sf::RenderWindow &window)
{
	switch (state)
	{
	case State::Menu:
		m_menu.Update(window);
		break;

	case State::Options:
		m_options.Update(window);
		break;

	case State::Pause:
		m_pause.Update(window);
		break;
	
	case State::Playing:
		m_play.Update(window);
		break;

	case State::End:
		m_finish.Update(window);
		break;
	}
}

void UserInterface::Draw(const State &state, sf::RenderWindow &window)
{
	switch (state)
	{
	case State::Menu:
		m_menu.Draw(window);
		break;
		
	case State::Options:
		m_options.Draw(window);
		break;

	case State::Pause:
		m_pause.Draw(window);
		break;

	case State::Playing:
		m_play.Draw(window);
		break;

	case State::End:
		m_finish.Draw(window);
		break;
	}
}

void UserInterface::CheckInput(Controller &ctr, Rocket &r, sf::RenderWindow &window, const sf::Event &evt)
{
	if (evt.type == sf::Event::KeyPressed)
	{
		switch (ctr.state)
		{
		case State::Menu:

			break;

		case State::Options:

			break;

		case State::Pause:
			if (evt.key.code == sf::Keyboard::Up)
			{
				ctr.state = State::Playing;
				ctr.cWorld->timer.restart();
			}
			if (evt.key.code == sf::Keyboard::M || evt.key.code == sf::Keyboard::Escape)
			{
				ctr.state = State::Menu;
				ResetUi(window, r, ctr);
			}
			break;

		case State::Playing:

			break;

		case State::End:
			if (evt.key.code == sf::Keyboard::Space)
			{
				m_finish.GetButton("nextB")->getClickEvent()(m_finish.GetButton("nextB"));
				break;
			}
			if (evt.key.code == sf::Keyboard::R)
			{
				m_finish.GetButton("resetB")->getClickEvent()(m_finish.GetButton("resetB"));
				break;
			}
			if (evt.key.code == sf::Keyboard::M)
			{
				m_finish.GetButton("menuB")->getClickEvent()(m_finish.GetButton("menuB"));
				break;
			}
			break;
		}
	}

	if (evt.type == sf::Event::MouseButtonPressed)
	{
		if (evt.key.code == sf::Mouse::Left)
		{
			switch (ctr.state)
			{
			case State::Menu:
				m_menu.CheckInput(window);
				break;

			case State::Options:
				m_options.CheckInput(window);
				break;

			case State::Pause:
				m_pause.CheckInput(window);
				break;

			case State::Playing:
				m_play.CheckInput(window);
				break;

			case State::End:
				m_finish.CheckInput(window);
				break;
			}
		}
	}
}

Widget& UserInterface::GetMenuWidget()
{
	return m_menu;
}

Widget& UserInterface::GetOptionsWidget()
{
	return m_options;
}

Widget& UserInterface::GetPauseWidget()
{
	return m_pause;
}

Widget& UserInterface::GetPlayWidget()
{
	return m_play;
}

Widget& UserInterface::GetFinishWidget()
{
	return m_finish;
}
