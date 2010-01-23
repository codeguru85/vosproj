package dto.security.video
{
	import mx.collections.ArrayCollection;
	[Bindable]
	[RemoteClass(alias="com.broadengate.vcms.entity.Keyword")]
	public class Keyword
	{
		
		public var wordId:int;
     	public var wordName:String;
    	public var createTime:Date;
    	public var videoNum:int;
     	public var videoInfos:UserInfo;
		public function Keyword()
		{
			//TODO: implement function
		}

	}
}