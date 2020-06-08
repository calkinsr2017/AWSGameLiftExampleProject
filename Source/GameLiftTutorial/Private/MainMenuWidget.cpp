// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include <WebBrowserWidget\Public\WebBrowser.h>

#include "IWebBrowserSingleton.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	bIsFocusable = true;

	//TODO - Robert, finish this off lol
	/*WebBrowser = (UWebBrowser*)GetWidgetFromName(TEXT("WebBrowser_Login"));

	if (WebBrowserSingleton != nullptr)
	{
		TOptional<FString> DefaultContext;
		TSharedPtr<IWebBrowserCookieManager> CookieManager = WebBrowserSingleton->GetCookieManager(DefaultContext);
		if (CookieManager.IsValid())
		{
			CookieManager->DeleteCookies();
		}
	}

	WebBrowser->LoadURL(LoginUrl);*/
}
